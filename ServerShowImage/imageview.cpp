#include "imageview.h"
#include "ui_imageview.h"

ImageView::ImageView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageView)
{
    ui->setupUi(this);
}

void ImageView::setPixmap(const QPixmap &image)
{
    ui->image->setPixmap(image);
}

ImageView::~ImageView()
{
    delete ui;
}
