#pragma once

#include <span>

#include <QLabel>
#include <QPixmap>
#include <QWidget>

#include <image/CoreTypes.hpp>

class ImageView : public QLabel {
    Q_OBJECT
public:
    void clear();
    void load(QSize size, const image::U8 *data);

    ImageView(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    virtual int heightForWidth(int width) const override;
    virtual QSize sizeHint() const override;
    QPixmap scaledPixmap() const;

public slots:
    void setPixmap ( const QPixmap & );
    void resizeEvent(QResizeEvent *);

private:
    QSize imageSize { 0, 0 };
    QPixmap pixmap;
};
