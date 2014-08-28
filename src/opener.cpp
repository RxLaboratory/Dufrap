#include "opener.h"
#include <QInputDialog>
#include <QUrl>
#include <QMessageBox>

Opener::Opener(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    //charger les favs
    QJsonArray favsArray = getFavs();

    for(int i = 0;i<favsArray.count();i++)
    {
        QJsonValue fav = favsArray[i];
        if (fav.toObject().value("type").toString() == "imageSequence")
        {
            favsList->addItem(fav.toObject().value("url").toString());
        }
    }
}

void Opener::setTitle(QString t)
{
    setWindowTitle(t);
}

QStringList Opener::getSequence(QString fichierBase)
{
    QStringList framesFound;

    QString fichier = "";
    //récupérer tous les fichiers du dossier
    fichierBase.replace("\\","/");
    QString dossierS = fichierBase.section("/",0,-2);
    QDir dossier(dossierS);
    QStringList fichiers = dossier.entryList(QStringList("*"),QDir::Files);

    //trouver le fichier sélectionné dedans
    int index = 0;
    for (int i = 0;i<fichiers.count();i++)
    {
        if (dossierS + "/" + fichiers[i] == fichierBase)
        {
            fichier = fichiers[i];
            index = i;
            break;
        }
    }

    //chercher le bloc de chiffres dans le fichier
    QRegExp re("(\\d+)");
    int pos = 0;
    int n = 0;
    QString test = "";
    bool found = false;

    while(pos >=0)
    {
        int i = re.indexIn(fichier,pos);
        QString frameNumberS = re.cap();
        int frameNumber = frameNumberS.toInt();

        //tester sur le suivant
        if (index < fichiers.count()-1)
        if (re.indexIn(fichiers[index+1],pos) == i)
        {
            if (frameNumber + 1 == re.cap().toInt())
            {
                test = fichier.replace(frameNumberS,"");
                found = true;
                pos = i;
                break;
            }
        }
        //tester sur précédent
        if (index > 0)
        if (re.indexIn(fichiers[index-1],pos) == i)
        {
            if (frameNumber + 1 == re.cap().toInt())
            {
                test = fichier.replace(frameNumberS,"");
                found = true;
                pos = i;
                break;
            }
        }
        pos = i + frameNumberS.count();
    }

    //trouvé, boucle sur les fichiers du dossier qui matchent le nom + d + nom
    if (found)
    {
        foreach(QString f, fichiers)
        {
            int i = re.indexIn(f,pos);
            if (i == pos)
            {
                QString filePath = dossierS + "/" + f;
                if (f.replace(re.cap(),"") == test) framesFound << filePath;
            }
        }
    }
    else
    {
        framesFound << fichierBase;
    }

    return framesFound;

}

void Opener::on_file_clicked()
{
    //demande les fichiers
    QString fichier = QFileDialog::getOpenFileName(this,"Ouvrir une séquence d'images");

        if (fichier != "")
        {
            frames = getSequence(fichier);

            if (addToFavs->isChecked())
            {
                //charger les anciens favs
                QJsonArray favsArray = getFavs();

                QJsonObject newFavs;
                newFavs.insert("type","imageSequence");
                newFavs.insert("url",frames[0]);
                favsArray.append(newFavs);

                setFavs(favsArray);
            }

            accept();
        }



}

void Opener::on_folder_clicked()
{

    QString dossier = QFileDialog::getExistingDirectory(this,"Ouvrir un dossier");
    if (dossier != "")
    {

            QDir dir(dossier);
            foreach(QString f,dir.entryList(QStringList("*"),QDir::Files))
            {
                frames << dossier + "/" + f;
            }

            if (addToFavs->isChecked())
            {
                //charger les anciens favs
                QJsonArray favsArray = getFavs();

                QJsonObject newFavs;
                newFavs.insert("type","imageSequence");
                newFavs.insert("url",frames[0]);
                favsArray.append(newFavs);

                setFavs(favsArray);
            }
            accept();
    }
}

QJsonArray Opener::getFavs()
{
    QJsonDocument favsDoc;
    QFile favsFile(QDir::homePath() + "/Dumep/favs.dufrap");
    if (favsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&favsFile);
        favsDoc = QJsonDocument::fromJson(in.readAll().toUtf8());
        favsFile.close();
    }
    if (favsDoc.isArray()) return favsDoc.array();
    else return QJsonArray();
}

void Opener::setFavs(QJsonArray favsArray)
{
    QJsonDocument favsDoc = QJsonDocument(favsArray);
    //écrire dans le fichier
    QFile favsFile(QDir::homePath() + "/Dumep/favs.dufrap");
    if (favsFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&favsFile);
        out << favsDoc.toJson();
        favsFile.close();
    }
}

QStringList Opener::getFrames()
{
    return frames;
}

void Opener::on_favsList_itemDoubleClicked()
{
    QJsonArray favs = getFavs();
    QJsonObject open = favs.at(favsList->currentRow()).toObject();
    if (open.value("type").toString() == "imageSequence")
    {
        QString fs = open.value("url").toString();
        frames = getSequence(fs);
    }
    accept();
}

void Opener::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        while(favsList->selectedItems().count() > 0)
        {
            int row = favsList->row(favsList->selectedItems().first());
            QListWidgetItem *item = favsList->takeItem(row);
            QJsonArray favs;
            favs = getFavs();
            favs.removeAt(row);
            setFavs(favs);
            delete item;
        }
        event->accept();
    }
    else if (event->key() == Qt::Key_Return)
    {
        QJsonArray favs = getFavs();
        foreach(QListWidgetItem *item,favsList->selectedItems())
        {
            QJsonObject open = favs.at(favsList->row(item)).toObject();
            if (open.value("type").toString() == "imageSequence")
            {
                QJsonArray fs = open.value("url").toArray();
                for (int i = 0;i<fs.count();i++)
                {
                    frames << fs[i].toString();
                }
            }
        }
        event->accept();
        accept();
    }
    else
    {
        QDialog::keyPressEvent(event);
    }
}

