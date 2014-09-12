#include "opener.h"
#include <QInputDialog>
#include <QUrl>
#include <QMessageBox>
#include <QMovie>
#include <QtDebug>
#include "params.h"

Opener::Opener(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    movie = false;

    refreshFavs();

    //charger les récents
    QJsonArray recentArray = getRecents();
    for(int i = 0;i<recentArray.count();i++)
    {
        QJsonValue recent = recentArray[i];
        if (recent.toObject().value("type").toString() == "imageSequence" || recent.toObject().value("type").toString() == "movie")
        {
            recentList->addItem(recent.toObject().value("url").toString());
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
    Params p;

    //demande les fichiers
    QString fichier = QFileDialog::getOpenFileName(this,"Ouvrir une séquence d'images",p.getLastBrowsed());

        if (fichier != "")
        {
            if (QMovie::supportedFormats().contains(fichier.section(".",-1).toLower().toUtf8()))
            {
                movie = true;
                movieFile = fichier;
            }
            else
            {
                frames = getSequence(fichier);
                movie = false;
            }

            //enregistrer le dossier utilisé
            QString d = fichier.section("/",0,-2);

            p.setLastBrowsed(d);

            addRecent();

            accept();
        }



}

void Opener::on_folder_clicked()
{
    Params p;

    QString dossier = QFileDialog::getExistingDirectory(this,"Ouvrir un dossier",p.getLastBrowsed());
    if (dossier != "")
    {

            QDir dir(dossier);
            foreach(QString f,dir.entryList(QStringList("*"),QDir::Files))
            {
                frames << dossier + "/" + f;
            }

            //recent
            addRecent();

            p.setLastBrowsed(dossier.section("/",0,-2));

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

QJsonArray Opener::getRecents()
{
    QJsonDocument recentDoc;
    QString recentName;
    recentName = "/Dumep/recent.dufrap";
    QFile recentFile(QDir::homePath() + recentName);
    if (recentFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&recentFile);
        recentDoc = QJsonDocument::fromJson(in.readAll().toUtf8());
        recentFile.close();
    }
    if (recentDoc.isArray()) return recentDoc.array();
    else return QJsonArray();
}

void Opener::setRecent(QJsonArray recentArray)
{
    QJsonDocument recentDoc = QJsonDocument(recentArray);
    //écrire dans le fichier
    QString recentName;
    recentName = "/Dumep/recent.dufrap";
    QFile recentFile(QDir::homePath() + recentName);
    if (recentFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&recentFile);
        out << recentDoc.toJson();
        recentFile.close();
    }
}

void Opener::addRecent()
{
    //recent
    QJsonArray recentArray = getRecents();
    QJsonObject newRecent;
    if (movie)
    {
        newRecent.insert("type","movie");
        newRecent.insert("url",movieFile);
    }
    else
    {
        newRecent.insert("type","imageSequence");
        newRecent.insert("url",frames[0]);
    }
    if (recentArray.contains(newRecent))
    {
        for(int i =0;i<recentArray.count();i++)
        {
            if (recentArray[i] == newRecent)
            {
                recentArray.removeAt(i);
                break;
            }
        }
    }

    recentArray.prepend(newRecent);

    Params p;
    if (recentArray.count() > p.getNumRecents())
    {
        recentArray.removeLast();
    }

    setRecent(recentArray);
}

void Opener::refreshFavs()
{
    //vider le tableau
    favsList->clear();
    //charger
    QJsonArray favsArray = getFavs();

    for(int i = 0;i<favsArray.count();i++)
    {
        QJsonValue fav = favsArray[i];
        if (fav.toObject().value("type").toString() == "imageSequence" || fav.toObject().value("type").toString() == "movie" )
        {
            favsList->addItem(fav.toObject().value("url").toString());
        }
    }
}

QStringList Opener::getFrames()
{
    return frames;
}

bool Opener::isMovie()
{
    return movie;
}

QString Opener::getMovie()
{
    return movieFile;
}

void Opener::on_favsList_itemDoubleClicked()
{
    QJsonArray favs = getFavs();
    QJsonObject open = favs.at(favsList->currentRow()).toObject();
    if (open.value("type").toString() == "imageSequence")
    {
        movie = false;
        QString fs = open.value("url").toString();
        frames = getSequence(fs);

        //recent
        addRecent();
    }
    else if (open.value("type").toString() == "movie")
    {
        movie = true;
        movieFile = open.value("url").toString();

        //recent
        addRecent();
    }
    accept();
}

void Opener::on_recentList_itemDoubleClicked(QListWidgetItem *item)
{
    QJsonArray recent = getRecents();
    QJsonObject open = recent.at(recentList->currentRow()).toObject();
    if (open.value("type").toString() == "imageSequence")
    {
        movie = false;
        QString fs = open.value("url").toString();
        frames = getSequence(fs);
    }
    else if (open.value("type").toString() == "movie")
    {
        movie = true;
        movieFile = open.value("url").toString();
    }
    accept();
}

void Opener::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        if (favsList->hasFocus())
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
        else if (recentList->hasFocus())
        {
            while(recentList->selectedItems().count() > 0)
            {
                int row = recentList->row(recentList->selectedItems().first());
                QListWidgetItem *item = recentList->takeItem(row);
                QJsonArray recent;
                recent = getRecents();
                recent.removeAt(row);
                setRecent(recent);
                delete item;
            }
            event->accept();
        }
    }
    else if (event->key() == Qt::Key_Return)
    {
        if (favsList->hasFocus())
        {
            QJsonArray favs = getFavs();
            QJsonObject open = favs.at(favsList->currentRow()).toObject();
            if (open.value("type").toString() == "imageSequence")
            {
                QString fs = open.value("url").toString();
                frames = getSequence(fs);
                movie = false;
                //recent
                addRecent();
            }
            else if (open.value("type").toString() == "movie")
            {
                movie = true;
                movieFile = open.value("url").toString();

                //recent
                addRecent();
            }
            event->accept();
            accept();
        }
        else if (recentList->hasFocus())
        {
            QJsonArray recent = getRecents();
            foreach(QListWidgetItem *item,recentList->selectedItems())
            {
                QJsonObject open = recent.at(recentList->row(item)).toObject();
                if (open.value("type").toString() == "imageSequence")
                {
                    QString fs = open.value("url").toString();
                    frames = getSequence(fs);
                    movie=false;
                }
                else if (open.value("type").toString() == "movie")
                {
                    movie = true;
                    movieFile = open.value("url").toString();
                }
            }
            event->accept();
            accept();
        }
    }
    else
    {
        QDialog::keyPressEvent(event);
    }
}

void Opener::on_addToFavs_clicked()
{
    //charger les anciens favs
    QJsonArray favsArray = getFavs();
    //charger les récents
    QJsonArray recent = getRecents();

    //pour chaque élément de la sélection
    foreach(QListWidgetItem *item,recentList->selectedItems())
    {
        QJsonObject newFavs = recent[recentList->row(item)].toObject();
        favsArray.append(newFavs);
    }
    setFavs(favsArray);

    refreshFavs();
}

void Opener::on_upButton_clicked()
{
    //charger les anciens favs
    QJsonArray favsArray = getFavs();
    QList<int> selection;
    //pour chaque élément de la sélection
    foreach(QListWidgetItem *item,favsList->selectedItems())
    {
        int index = favsList->row(item);
        if (index > 0 && index <= favsList->count()-1)
        {
            favsArray.insert(index-1,favsArray.takeAt(index));
            index--;
        }
        selection << index;
    }
    setFavs(favsArray);
    refreshFavs();
    //restore selection
    foreach(int index,selection)
    {
        favsList->item(index)->setSelected(true);
    }
}

void Opener::on_downButton_clicked()
{
    //charger les anciens favs
    QJsonArray favsArray = getFavs();
    QList<int> selection;
    //pour chaque élément de la sélection
    for (int i = favsList->selectedItems().count()-1;i>=0;i--)
    {
        QListWidgetItem *item = favsList->selectedItems()[i];
        int index = favsList->row(item);
        if (index >= 0 && index < favsList->count()-1)
        {
            favsArray.insert(index+1,favsArray.takeAt(index));
            index++;
        }
        selection << index;
    }
    setFavs(favsArray);
    refreshFavs();
    //restore selection
    for (int i = selection.count()-1;i>=0;i--)
    {
        int index = selection[i];
        favsList->item(index)->setSelected(true);
    }
}
