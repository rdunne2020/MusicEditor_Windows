#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QRegularExpression>
#include "flaccontainer.h"
#include "editor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void editTitle(QString& rSongName, int trackNum, bool continuous);
    void parseFiles(QStringList &rFileList);

private slots:
    void on_actionOpen_Folder_triggered();
    void fill_fileList(QStringList& rFilesForList);

    void on_actionRefresh_triggered();

    unsigned long writeNewFiles();

    void launchManualEditor();

    void receiveManuallyChangedName(QString newName, int rowChanged, Editor::SONG_DATA_TYPE data_type);

    //void handleContinuousCheck();

signals:
    void send_songs(QStringList& rSelectedSongs);
    void send_songname_status(const QString& filename);
    void update_progress(int value);

private:
    Ui::MainWindow *ui;
    QDir _songDir;
    QStringList _filesInDir;
    QStringList _fullFilePath;
    QString _fileExt = "";
    // Vector that holds in depth info for all flac files read in
    // std::vector<FlacContainer> _listOfFlacFiles;
};
#endif // MAINWINDOW_H
