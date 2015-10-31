/*
This software is part of qtcsdr.

Copyright (c) 2015, Andras Retzler <randras@sdr.hu>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ANDRAS RETZLER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "qmyspectrumwidget.h"
#include <math.h>
#include <QDebug>
#include <QMouseEvent>

QMySpectrumWidget::QMySpectrumWidget(QWidget *parent) :
    QWidget(parent), FFTSize(2048), sampleRate(2400000), offsetFreq(0), filterLowCut(0), filterHighCut(0)
{
    oneLineOfSpectrum=new QImage(FFTSize, 1, QImage::Format_ARGB32);
    reinit();
}

void QMySpectrumWidget::reinit()
{
    spectrumImage=new QImage(this->size(), QImage::Format_ARGB32);
    for(int i=0;i<spectrumImage->bytesPerLine()*spectrumImage->height()/sizeof(unsigned);i++)
        *(((unsigned*)spectrumImage->bits())+i)=0;
}

void QMySpectrumWidget::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.drawImage(0,0,*spectrumImage);

    int halfWidth = this->width()/2;
    int rectX = halfWidth+((float)(this->offsetFreq+this->filterLowCut)/this->sampleRate)*this->width();
    int rectW = ((float)(this->filterHighCut-this->filterLowCut)/this->sampleRate)*this->width();
    //qDebug() << "pe" << rectX << rectW;
    p.fillRect(rectX,0,rectW,this->height(),QColor::fromRgbF(1,1,1,0.3));
}

void QMySpectrumWidget::mouseReleaseEvent(QMouseEvent* event)
{

    int halfWidth = this->width()/2;
    emit shiftChanged(  ((event->x()-halfWidth)/(float)this->width()) * this->sampleRate  );
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
         QRgb* rgba = ((QRgb*)oneLineOfSpectrum->bits())+i;
         QColor col = QColor::fromRgba(*rgba);
         col.setRed((rawColor&0xff000000)>>24);
         col.setGreen((rawColor&0xff0000)>>16);
         col.setBlue((rawColor&0xff00)>>8);
         col.setAlpha(rawColor&0xff);
         *rgba = col.rgba();
     }
     QImage scaledImage = oneLineOfSpectrum->scaled(this->width(),1,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
     for(int i=0;i<this->width();i++)
     {
         ((QRgb*)spectrumImage->bits())[i] = ((QRgb*)scaledImage.bits())[i];
     }
     from->remove(0,FFTSize*sizeof(unsigned));
     this->repaint();
     return true;
 }
