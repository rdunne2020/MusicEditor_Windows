#ifndef EDITOR_H
#define EDITOR_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class Editor;
}

class Editor : public QDialog
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    void setupList(const QStringList& formattedNames, const QStringList& discNumbers, const QStringList& trackNumbers);
    ~Editor();

    enum SONG_DATA_TYPE
    {
        SONG_TITLE,
        DISC_NUM,
        TRACK_NUM
    };

private slots:
    void editMetadata(QListWidgetItem* pUpdatedSong);
    void getChangedSongRow(int rowChanged);
    void getChangedDiscRow(int rowChanged);
    void getChangedTrackRow(int rowChanged);
    void propagateChange();

signals:
    void sendDataToMain(QString newDataName, int songNum, SONG_DATA_TYPE data_type);

private:
    Ui::Editor *ui;
    std::vector<int> _changedNameRows;
    std::vector<int> _changedDiscRows;
    std::vector<int> _changedTrackRows;
};

#endif // EDITOR_H
