#include "lfiledialog.h"

LFileDialog::LFileDialog(QWidget *parent, const QString &caption, const QString &directory, const QString &filter )
	:_fileDialog(new QFileDialog(parent, caption, directory, filter))
{
}

LFileDialog::~LFileDialog()
{
	delete _fileDialog;
}

QFileDialog * LFileDialog::fileDialog() const
{
	return _fileDialog;
}

QSize LFileDialog::sizeHint() const
{
	return _fileDialog->sizeHint();
}

QString LFileDialog::getOpenFileName(QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options)
{
	return QFileDialog::getOpenFileName(parent, caption, dir, filter,selectedFilter, options);
}

QUrl LFileDialog::getOpenFileUrl(QWidget * parent, const QString & caption, const QUrl & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options, const QStringList & supportedSchemes)
{
	return QFileDialog::getOpenFileUrl(parent, caption, dir, filter, selectedFilter, options, supportedSchemes);
}

QString LFileDialog::getSaveFileName(QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options)
{
	return QFileDialog::getSaveFileName(parent, caption, dir, filter, selectedFilter, options);
}

QUrl LFileDialog::getSaveFileUrl(QWidget * parent, const QString & caption, const QUrl & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options, const QStringList & supportedSchemes)
{
	return QFileDialog::getSaveFileUrl(parent, caption, dir, filter, selectedFilter, options, supportedSchemes);
}

QString LFileDialog::getExistingDirectory(QWidget * parent, const QString & caption, const QString & dir, QFileDialog::Options options)
{
	return QFileDialog::getExistingDirectory(parent, caption, dir, options);
}

QUrl LFileDialog::getExistingDirectoryUrl(QWidget * parent, const QString & caption, const QUrl & dir, QFileDialog::Options options, const QStringList & supportedSchemes)
{
	return QFileDialog::getExistingDirectoryUrl(parent, caption, dir, options, supportedSchemes);
}

QStringList LFileDialog::getOpenFileNames(QWidget * parent, const QString & caption, const QString & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options)
{
	return QFileDialog::getOpenFileNames(parent, caption, dir, filter, selectedFilter, options);
}

QList<QUrl> LFileDialog::getOpenFileUrls(QWidget * parent, const QString & caption, const QUrl & dir, const QString & filter, QString * selectedFilter, QFileDialog::Options options, const QStringList & supportedSchemes)
{
	return QFileDialog::getOpenFileUrls(parent, caption, dir, filter, selectedFilter, options, supportedSchemes);
}

LFileDialog::LFileDialog(const LFileDialogArgs & args)
{
	init(args);
}

void LFileDialog::init(const LFileDialogArgs & args)
{
}
