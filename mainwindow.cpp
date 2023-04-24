/****************************************************************************
 * Music Editor: Let's see if this works
 * TODO:
 * Add a logger
 * Use dialogs to show errors
****************************************************************************/

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "songlistobject.h"
#include "copystatusdialog.h"
#include <iostream>
#include <QDir>
#include <QFile>
#include <memory>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->saveButton->setStyleSheet("background-color: rgb(87,87,87); color: white; border: 1px solid grey; border-radius: 4px");
    ui->saveAsButton->setStyleSheet("background-color: rgb(87,87,87); color: white; border: 1px solid grey; border-radius: 4px");
    ui->manualButton->setStyleSheet("background-color: rgb(87,87,87); color: white; border: 1px solid grey; border-radius: 4px");
    connect(ui->saveButton, &QPushButton::pressed, this, &MainWindow::writeNewFiles);
    connect(ui->saveAsButton, &QPushButton::pressed, this, &MainWindow::writeNewFiles);
    connect(ui->manualButton, &QPushButton::pressed, this, &MainWindow::launchManualEditor);
    connect(this, &MainWindow::send_songs, this, &MainWindow::fill_fileList);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::launchManualEditor()
{
    if(_filesInDir.count() < 1)
    {
        std::cout << "Please open a directory with songs in it first" << std::endl;
    }
    else
    {
        std::unique_ptr<Editor> pManualEditor = std::make_unique<Editor>();
        QStringList formattedFileNames;
        QStringList discNumbers;
        QStringList trackNumbers;
        for(int i = 0; i < ui->formattedNames->count(); ++i)
        {
            QListWidgetItem* pSongItem = ui->formattedNames->item(i);
            SongListObject* pSongData = static_cast<SongListObject*>(ui->formattedNames->itemWidget(pSongItem));
            formattedFileNames.append(pSongData->getSongName());
            discNumbers.append(pSongData->getDiscNum());
            trackNumbers.append(pSongData->getSongNum());
        }
        pManualEditor->setupList(formattedFileNames, discNumbers, trackNumbers);
        connect(pManualEditor.get(), &Editor::sendDataToMain, this, &MainWindow::receiveManuallyChangedName);
        pManualEditor->exec();
        disconnect(pManualEditor.get(), &Editor::sendDataToMain, this, &MainWindow::receiveManuallyChangedName);
        pManualEditor.release();
    }
}

void MainWindow::receiveManuallyChangedName(QString newName, int rowChanged, Editor::SONG_DATA_TYPE data_type)
{
    QListWidgetItem* pItemToUpdate = ui->formattedNames->item(rowChanged);
    SongListObject* pSongObjectToUpdate = static_cast<SongListObject*>(
        ui->formattedNames->itemWidget(pItemToUpdate));
    if (data_type == Editor::SONG_TITLE)
    {
        pSongObjectToUpdate->setSongName(newName);
    }
    else if (data_type == Editor::DISC_NUM)
    {
        pSongObjectToUpdate->setDiscNum(newName);
    }
    else
    {
        pSongObjectToUpdate->setSongNum(newName);
    }
}

/*****************************************
 * Writes files
 * Loops thru all the files, writes the old data to the new file names
 * Returns the number of bytes written, zero means an error
*****************************************/
unsigned long MainWindow::writeNewFiles()
{
    if(_filesInDir.size() < 1 || ui->formattedNames->count() < 1)
    {
        std::cout << "Open Directory With Music Files in it First" << std::endl;
        return 0;
    }
    else
    {
        QString saveDir = _songDir.absolutePath();
        QString buttonText = static_cast<QPushButton*>(QObject::sender())->text();
        bool pressedSaveAs = (buttonText == "Save As");

        // If save as was activated, open a dialog to select a new directory
        if(pressedSaveAs)
        {
            QString new_directory_selected = QFileDialog::getExistingDirectory(this, "Save New Files As", "./", QFileDialog::Option::ShowDirsOnly);
            if(new_directory_selected.isEmpty())
            {
                std::cout << "Select a directory to save your files to" << std::endl;
                return 0;
            }
            saveDir = new_directory_selected;
        }

        std::cout << "Saving new songs to : " << saveDir.toStdString() << std::endl;
        unsigned long totalNumMbWritten = 0;

        // Create progress dialog
        copystatusdialog progress_bar;
        connect(this, &MainWindow::send_songname_status, &progress_bar, &copystatusdialog::showCopyingFile);
        connect(this, &MainWindow::update_progress, progress_bar.getProgressBar(), &QProgressBar::setValue);
        progress_bar.show();
        for(int i = 0; i < ui->formattedNames->count(); ++i)
        {
            // Get the filename to read the data into memory and write it
            QString unformattedName = ui->unformattedNames->item(i)->text();
            QListWidgetItem* pCurrentItem = ui->formattedNames->item(i);
            SongListObject* pSongObject = static_cast<SongListObject*>(ui->formattedNames->itemWidget(pCurrentItem));
            QString new_song_title = pSongObject->getSongName();
            QString new_disc_num = pSongObject->getDiscNum();
            QString new_track_num = pSongObject->getSongNum();

            // Update the Progress Bar:
            emit(send_songname_status(new_song_title));
            double prog_perc_decimal = static_cast<double>(i + 1) / static_cast<double>(ui->formattedNames->count());
            int progress_percentage = std::round(prog_perc_decimal * 100);
            emit(update_progress(progress_percentage));
            // This needs to be called when using a modal and show() in order to handle drawing and updates
            QApplication::processEvents();
            try
            {
                pSongObject->flac_data.set_metadata("TITLE", new_song_title.toStdString());
                pSongObject->flac_data.set_metadata("DISCNUMBER", new_disc_num.toStdString());
                pSongObject->flac_data.set_metadata("TRACKNUMBER", new_track_num.toStdString());
            }
            catch(const FlacException& e)
            {
                std::cerr << e.what() << std::endl;
            }

            // Here is where we need to update the FLAC data with new stuff
            // TODO: Clean up path generation, has to be a better way than using + operators

            QFile old_song_file(_songDir.absolutePath() + "/" + unformattedName + "." + _fileExt);
            QString newSongName = saveDir + "/" + new_song_title + "." + _fileExt;
            if (old_song_file.copy(newSongName))
            {
                totalNumMbWritten = old_song_file.size();
            }
        }
        return totalNumMbWritten;
    }
}

void MainWindow::editTitle(QString &rSongName, int trackNum, bool continuous)
{
    // Capitalize the start of every appropriate word, else lowercase it
    // TODO: Let user input the file separators and create the regex string from that
    QRegularExpression split_re("( |_)");
    QStringList wordsInTitle = rSongName.split(split_re, Qt::SplitBehaviorFlags::SkipEmptyParts);

    bool alreadyHasArrow = false;

    // Used for detecting when it's the first word in a song title and not lowercasing it if need be
    int word_count = 0;
    for(auto& word : wordsInTitle)
    {
        // TODO: Clean this up
        QRegularExpression badUppercase("(^a$|^an$|^and$|^but$|^for$|^to$|^at$|^by$)", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch toLowercase = badUppercase.match(word);
        if(toLowercase.hasMatch())
        {
            // Don't lowercase it if it's the first word in the title
            if(word_count > 0)
            {
                word.replace(0,1, word[0].toLower());
            }
        }
        else
        {
            // Replace the first char in Word with an upper case variant
            word.replace(0,1, word[0].toUpper());
        }
        ++word_count;
        // If the song title already has an arrow,
        if(word.contains(">"))
        {
            alreadyHasArrow = true;
        }
    }

    if(continuous && !alreadyHasArrow)
    {
        wordsInTitle.append(">");
    }

    rSongName = wordsInTitle.join(" ");
}

// This is for refreshing the file list, typically just adds in a trailing > in the list widget
void MainWindow::parseFiles(QStringList &rFileList)
{
    QStringList formattedNames;
    // Set the song continuation to off for all the songs by default
    for(int i = 0; i < ui->unformattedNames->count(); ++i)
    {
        QListWidgetItem *pUnformattedSong = ui->unformattedNames->item(i);
        if(pUnformattedSong && pUnformattedSong->checkState() == Qt::Checked)
        {
            QListWidgetItem* pFormattedSong = ui->formattedNames->item(i);
            SongListObject* pSongObj = static_cast<SongListObject*>(ui->formattedNames->itemWidget(pFormattedSong));
            QString formattedName = pSongObj->getSongName() + " >";
            pSongObj->setSongName(formattedName);
        }
    }
    ui->formattedNames->addItems(formattedNames);
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    //QStringList directory_selected;
    QDir directory_selected = QFileDialog::getExistingDirectory(this);
    std::cout << "Path: " << directory_selected.absolutePath().toStdString() << std::endl;
    QStringList audio_file_filter;
    // TODO: Add Support for more filetypes, but right now just flac, see TODO in flaccontainer.
    audio_file_filter << "*.flac";
    QStringList files_in_dir = directory_selected.entryList(audio_file_filter, QDir::Files);
    _songDir = directory_selected;
    for(auto& rFile : files_in_dir)
    {
        // Split file names
        QStringList tmpSplit = rFile.split(".");
        _fileExt = tmpSplit.at(1);
        rFile = tmpSplit.at(0);

        // Setting it to metadata title if it exists rFile = tmpSplit.at(0);
        std::cout << rFile.toStdString() << std::endl;
    }
    _filesInDir = files_in_dir;
    emit(send_songs(_filesInDir));
}

void MainWindow::fill_fileList(QStringList& rFilesForList)
{
    QStringList formattedNames;
    // Clear the current lists
    ui->unformattedNames->clear();
    ui->formattedNames->clear();
    ui->unformattedNames->addItems(rFilesForList);
    QRegularExpression continuous(".*>", QRegularExpression::DotMatchesEverythingOption);
    // Set the song continuation to off for all the songs by default
    for(int i = 0; i < ui->unformattedNames->count(); ++i)
    {
        QListWidgetItem* pSongItem = ui->unformattedNames->item(i);
        pSongItem->setFlags(pSongItem->flags() | Qt::ItemIsUserCheckable);
        // Create the FlacContainers for each song
        FlacContainer next_song((_songDir.absolutePath().toStdString() + "/" +
            rFilesForList.at(i).toStdString() + "." + _fileExt.toStdString()));
        bool valid_flac = true;
        try
        {
            next_song.set_vorbis_block();
        }
        catch (const FlacException& e)
        {
            valid_flac = false;
            std::cerr << e.what() << std::endl;
        }
        if (valid_flac)
        {
            std::string flac_title = next_song.get_metadata_tag("TITLE").second;
            std::string song_artist = next_song.get_metadata_tag("ARTIST").second;
            std::string track_num = next_song.get_metadata_tag("TRACKNUMBER").second;
            std::string disc_num = next_song.get_metadata_tag("DISCNUMBER").second;
            next_song._title = flac_title;
            next_song._artist = song_artist;
            if (track_num != "NULL" && disc_num != "NULL")
            {
                next_song._track_num = std::stoi(track_num);
                next_song._disc_num = std::stoi(disc_num);
            }
            else
            {
                // Use i+1 to account for zero indexing
                next_song._track_num = i+1;
                next_song._disc_num = 0;
            }
        }
        QString qSongName;
        if(next_song._title != "NULL")
        {
            qSongName = next_song._title.c_str();
            
        }
        // FLAC Title tag wasn't read, use the raw file name
        else
        {
            qSongName = rFilesForList.at(i);
        }
        QRegularExpressionMatch isContinuous = continuous.match(qSongName);
        bool continuous_song = true;
        // We only want the user to be able to set continuous for files without arrows
        if (!isContinuous.hasMatch())
        {
            pSongItem->setCheckState(Qt::Unchecked);
            continuous_song = false;
        }
        else
        {
            pSongItem->setCheckState(Qt::Checked);
        }

        editTitle(qSongName, i, continuous_song);
        SongListObject* pSong_container = new SongListObject(ui->formattedNames);
        pSong_container->setSongName(qSongName);
        pSong_container->setSongNum(QString::number(next_song._track_num));
        pSong_container->setDiscNum(QString::number(next_song._disc_num));
        pSong_container->setArtist(QString(next_song._artist.c_str()));
        pSong_container->flac_data = next_song;
        
        // This is how to use a custom widget inside a QListWidget
        QListWidgetItem* pTmpSongItem = new QListWidgetItem();
        QSize widget_size = QSize(pSong_container->width(),pSong_container->height());
        pTmpSongItem->setSizeHint(widget_size);
        ui->formattedNames->addItem(pTmpSongItem);
        ui->formattedNames->setItemWidget(pTmpSongItem, pSong_container);
    }
}

void MainWindow::on_actionRefresh_triggered()
{
    if(ui->unformattedNames->count() > 0 && ui->formattedNames->count() > 0)
    {
        parseFiles(_filesInDir);
    }
}
