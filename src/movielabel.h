#ifndef MOVIELABEL_H
#define MOVIELABEL_H

#include <QLabel>

class MovieLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MovieLabel(QWidget *parent = 0);
    void setMovieFile(QString f);

signals:

public slots:

private:

protected:


};

#endif // MOVIELABEL_H
