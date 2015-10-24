#include "qmyspectrumwidget.h"
#include <math.h>
#include <QDebug>

QMySpectrumWidget::QMySpectrumWidget(QWidget *parent) :
    QWidget(parent)
{
    reinit();
}

void QMySpectrumWidget::reinit()
{
    spectrumImage=new QImage(FFTSize, this->size().height(), QImage::Format_ARGB32);
    for(int i=0;i<spectrumImage->bytesPerLine()*spectrumImage->height()/sizeof(unsigned);i++)
        *(((unsigned*)spectrumImage->bits())+i)=0;
}

void QMySpectrumWidget::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    QImage scaledImage = spectrumImage->scaled(this->width(),this->height(),Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    p.drawImage(0,0,scaledImage);

    int halfWidth = this->width()/2;
    int rectX = halfWidth+((float)(this->offsetFreq+this->filterLowCut)/this->sampleRate)*halfWidth;
    int rectW = ((float)(this->filterHighCut-this->filterLowCut)/this->sampleRate)*this->width();
    //qDebug() << "pe" << rectX << rectW;
    p.fillRect(rectX,0,rectW,this->height(),QColor::fromRgbF(1,1,1,0.3));
}

void QMySpectrumWidget::resizeEvent(QResizeEvent* event)
{
    reinit();
}

void QMySpectrumWidget::shiftImageOneLineDown()
{
    for(int i=spectrumImage->height()-1;i>0;i--)
    {
        unsigned* ptrDst = (unsigned*)spectrumImage->scanLine(i);
        unsigned* ptrSrc = (unsigned*)spectrumImage->scanLine(i-1);
        //qDebug() << "h" << spectrumImage->height() << "i" << i << ptrSrc << ptrDst;
        for(int j=0;j<spectrumImage->bytesPerLine()/sizeof(unsigned);j++) *(ptrDst++)=*(ptrSrc++);

    }
}

unsigned colorScale[] = {0x000000ff,0x2e6893ff, 0x69a5d0ff, 0x214b69ff, 0x9dc4e0ff,  0xfff775ff, 0xff8a8aff, 0xb20000ff};

unsigned colorBetween(unsigned first, unsigned second, float percent)
{
    unsigned output=0;
    for(int i=0;i<4;i++)
    {
        unsigned add = ( (unsigned)((first&(0xff<<(i*8)))*percent) + (unsigned)((second&(0xff<<(i*8)))*(1-percent)) ) & (0xff<<(i*8));
        output |= add;
    }
    //qDebug() << QString::number(output,16);
    return output;
}


unsigned waterfallMakeColor(float db)
{
    float minValue=-115; //in dB
    float maxValue=0;
    if(db<minValue) db=minValue;
    if(db>maxValue) db=maxValue;
    float fullScale=maxValue-minValue;
    float relativeValue=db-minValue;
    float valuePercent=relativeValue/fullScale;
    float percentForOneColor=1.0/(sizeof(colorScale)/sizeof(unsigned)-1);
    int index=floor(valuePercent/percentForOneColor);
    float remain=(valuePercent-percentForOneColor*index)/percentForOneColor;
    return colorBetween(colorScale[index+1],colorScale[index],remain);
}

unsigned chEndianness(unsigned i)
{
    return ((i&0xff)<<24) | ((i&0xff00)<<8) | ((i&0xff0000)>>8) | ((i&0xff000000)>>24);
}

 bool QMySpectrumWidget::takeOneWaterfallLine(QByteArray* from)
 {
     float* fdata = (float*)from->data();
     if(from->length()<=FFTSize*sizeof(unsigned)) return false;
     shiftImageOneLineDown();
     for(int i=0;i<FFTSize;i++)
     {
         unsigned rawColor = waterfallMakeColor(fdata[i]);
         QRgb* rgba = ((QRgb*)spectrumImage->bits())+i;
         QColor col = QColor::fromRgba(*rgba);
         col.setRed((rawColor&0xff000000)>>24);
         col.setGreen((rawColor&0xff0000)>>16);
         col.setBlue((rawColor&0xff00)>>8);
         col.setAlpha(rawColor&0xff);
         *rgba = col.rgba();
     }
     from->remove(0,FFTSize*sizeof(unsigned));
     this->repaint();
     return true;
 }
