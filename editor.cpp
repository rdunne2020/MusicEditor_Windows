#include "editor.h"
#include "ui_editor.h"

Editor::Editor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
    // Since the text is white as set in main.cpp, make the edit box black
    ui->songNames->setStyleSheet("QLineEdit{ background:black; };");
    ui->discNums->setStyleSheet("QLineEdit{ background:black; };");
    ui->trackNums->setStyleSheet("QLineEdit{ background:black; };");
    connect(this->ui->songNames, &QListWidget::itemDoubleClicked, this, &Editor::editMetadata);
    connect(this->ui->songNames, &QListWidget::currentRowChanged, this, &Editor::getChangedSongRow);
    connect(this->ui->discNums, &QListWidget::itemDoubleClicked, this, &Editor::editMetadata);
    connect(this->ui->discNums, &QListWidget::currentRowChanged, this, &Editor::getChangedDiscRow);
    connect(this->ui->trackNums, &QListWidget::itemDoubleClicked, this, &Editor::editMetadata);
    connect(this->ui->trackNums, &QListWidget::currentRowChanged, this, &Editor::getChangedTrackRow);

    connect(this, &QDialog::accepted, this, &Editor::propagateChange);
}

void Editor::setupList(const QStringList &formattedNames, const QStringList& discNumbers, const QStringList& trackNumbers)
{
    for (uint32_t i = 0; i < formattedNames.count(); ++i)
    {
        ui->songNames->addItem(formattedNames.at(i));
        ui->discNums->addItem(discNumbers.at(i));
        ui->trackNums->addItem(trackNumbers.at(i));
    }
}

void Editor::editMetadata(QListWidgetItem* pUpdatedSong)
{
    pUpdatedSong->setFlags(pUpdatedSong->flags() | Qt::ItemIsEditable);
}

void Editor::getChangedSongRow(int rowChanged)
{
    _changedNameRows.push_back(rowChanged);
}

void Editor::getChangedDiscRow(int rowChanged)
{
    _changedDiscRows.push_back(rowChanged);
}

void Editor::getChangedTrackRow(int rowChanged)
{
    _changedTrackRows.push_back(rowChanged);
}

void Editor::propagateChange()
{
    for(auto row : _changedNameRows)
    {
        QString newFileName = ui->songNames->item(row)->text();
        emit(sendDataToMain(newFileName, row, SONG_TITLE));
    }
    for (auto row : _changedDiscRows)
    {
        QString newDiscNum = ui->discNums->item(row)->text();
        emit(sendDataToMain(newDiscNum, row, DISC_NUM));
    }
    for (auto row : _changedTrackRows)
    {
        QString newTrackNum = ui->trackNums->item(row)->text();
        emit(sendDataToMain(newTrackNum, row, TRACK_NUM));
    }
    _changedNameRows.clear();
}

Editor::~Editor()
{
    delete ui;
}
