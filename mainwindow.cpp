#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString path = QFileDialog::getExistingDirectory(this, tr("Wybierz folder..."));

    path += "/";

    qDebug() << path;

    QString path_log = path + "log_ftp.txt";
    QString path_log_dir = path + "log_dir.txt";

    QFile log(path_log);
    QFile log_dir(path_log_dir);
    if(!log.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Nie można otworzyć pliku log.txt";
    }

    if(!log_dir.open(QIODevice::WriteOnly | QIODevice::Text))
        qDebug() << "Nie można otworzyć pliku log_dir.txt";

    QTextStream out(&log);
    QTextStream out_dir(&log_dir);

    QString lnk = path;

    QDir dir(lnk);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks);             // Ustawienie filtrów
    dir.setSorting(QDir::Name);                                                                     // Ustawienie sortowania

    QFileInfoList dir_list = dir.entryInfoList();                                                   // Utworzenie listy folderów

    for(int i = 0; i < dir_list.size(); ++i)
    {
        QFileInfo dirInfo = dir_list.at(i);

        qDebug() << dirInfo.fileName();

        out_dir << dirInfo.fileName() << endl;

        QString lnk_file = lnk + dirInfo.fileName();

        QDir dir2(lnk_file);

        dir2.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir2.setSorting(QDir::Name);

        QFileInfoList file_list = dir2.entryInfoList();

        for(int j = 0; j < file_list.size(); ++j)
        {
            QFileInfo fileInfo = file_list.at(j);
            QString lnk_file_sum = lnk_file + "/" + fileInfo.fileName();
            QFile *file = new QFile(lnk_file_sum);
            file->open(QIODevice::ReadOnly);

            QByteArray byte = file->read(524288);

            quint16 crcSum = qChecksum(byte.data(), byte.length());

            file->close();

            qDebug() << "\t" << fileInfo.fileName();

            out << dirInfo.fileName() << "/" << fileInfo.fileName() << "\t" << fileInfo.size() << "\t" << QString::number(crcSum, 16) << endl;
        }
    }

    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);

    QFileInfoList file_default_folder = dir.entryInfoList();

    for(int k = 0; k < file_default_folder.size(); ++k)
    {
        QFileInfo fileInfo = file_default_folder.at(k);
        QFile *file = new QFile(lnk + fileInfo.fileName());
        file->open(QIODevice::ReadOnly);
        QByteArray byte = file->read(524288);

        quint16 crcSum = qChecksum(byte.data(), byte.length());

        file->close();

        if(fileInfo.fileName() != "log_ftp.txt")
            out << fileInfo.fileName() << "\t" << fileInfo.size() << "\t" << QString::number(crcSum, 16) << endl;
    }

    log.close();
    log_dir.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}
