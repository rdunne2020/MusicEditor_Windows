#include "copystatusdialog.h"
#include "ui_copystatusdialog.h"

copystatusdialog::copystatusdialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::copystatusdialog())
{
	ui->setupUi(this);
	this->setWindowTitle("File Save Progress");
	setWindowFlags(Qt::Dialog);
	this->setModal(true);
	this->setWindowModality(Qt::NonModal);
}

copystatusdialog::~copystatusdialog()
{
	delete ui;
}

QProgressBar* copystatusdialog::getProgressBar()
{
	return ui->progressBar;
}

void copystatusdialog::showCopyingFile(const QString& filename)
{
	ui->new_song_name->setText(filename);
}
