#pragma once

#include <QDialog>
#include <QProgressBar>
QT_BEGIN_NAMESPACE
namespace Ui { class copystatusdialog; };
QT_END_NAMESPACE

class copystatusdialog : public QDialog
{
	Q_OBJECT

public:
	copystatusdialog(QWidget *parent = nullptr);
	~copystatusdialog();
	void setProgress();
	QProgressBar* getProgressBar();

private:
	Ui::copystatusdialog *ui;

public slots:
	void showCopyingFile(const QString& filename);
};
