#include "terrain.h"

Terrain::Terrain()
{

}

Terrain::Terrain(int length, int width, int nbPointWidth, int nbPointLength)
{
    this->width = width;
    this->length = length;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < length; j++)
        {
            height.append(0);
            dirt.append(0);
            norm.append(QVector3D());
            avgSlope.append(0);
        }
    }
}

double Terrain::getHeightAt(int x, int y)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        return height[(y * width) + x];
    }
    else
    {
        return -1;
    }
}

double Terrain::getDirtAt(int x, int y)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        return dirt[(y * width) + x];
    }
    else
    {
        return -1;
    }
}

double Terrain::getTemperAt(int x, int y)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        return temper[(y * width) + x];
    }
    else
    {
        return -1;
    }
}

QVector3D Terrain::getNormAt(int x, int y)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        return norm[(y * width) + x];
    }
    else
    {
        return QVector3D();
    }
}

double Terrain::getAvgSlope(int x, int y)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        return avgSlope[(y * width) + x];
    }
    else
    {
        return -1;
    }
}

void Terrain::setNormAt(int x, int y,QVector3D value)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        norm[(y * width) + x] = value;
    }
}

void Terrain::setTemperAt(int x, int y,double value)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        temper[(y * width) + x] = value;
    }
}

void Terrain::setHeightAt(int x, int y, double z)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        height[(y * width) + x] = z;
    }
}

void Terrain::setDirtAt(int x, int y, double dirtValue)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        dirt[(y * width) + x] = dirtValue;
    }
}

void Terrain::setAvgSlope(int x, int y, double value)
{
    if(x >= 0 && y >= 0 && x < width && y < length)
    {
        avgSlope[(y * width) + x] = value;
    }
}

void Terrain::generateTerrainFromNoise(double freq, double amp,int start, boolean ridge)
{
    double i, j;
    double h;
    int periode = 500;
    float ridge_val;
    Perlin perlin = Perlin();

    for(i = 0; i < width; i++)
    {
        for(j = 0; j < length; j++)
        {

            ridge_val=amp *perlin.noise((i+start+1500)*freq /periode , (j+start+1500)*freq /periode);
            h=amp * (perlin.noise((i+start)*freq /periode , (j+start)*freq /periode));

            if(ridge){
                if(h<ridge_val){
                    //height[(j*width)+i] -= (h-ridge_val)*2;
                    height[(j*width)+i] += h;
                }
                else{
                    height[(j*width)+i] += ridge_val;
                }
            }
            else{
                height[(j*width)+i] += h;
            }
        }
    }
    initGradTemper();
    //saveAsImage("map.raw");
}

void Terrain::loadFromHeightMap(QString name)
{
    QImage map = QImage(name);
    int i, j;

    map.convertToFormat(QImage::Format_Indexed8);

    width = map.size().width();
    length = map.size().height();

    for(i = 0; i < width; i++)
    {
        for(j = 0; j < length; j++)
        {
            height.append(qGray(map.pixel(i,j)));
        }
    }
}

void Terrain::display()
{
    int i = 0;
    glColor3f(0.0, 1.0, 0.0);

    for(i = 0; i < height.size(); i++)
    {
        glBegin(GL_POINTS);

        glVertex3f((((double)i/(double)length)/(double)length)-0.5
                   ,((double)(i%length)/(double)width)-0.5
                   ,height[i]);
        glEnd();
    }
}

void Terrain::saveAsImage(QString name)
{
    int i, gray;
    QImage map = QImage(width, length, QImage::Format_RGB32);

    for(i = 0; i < height.size(); i++)
    {
        gray = qGray((height[i] + 0.5) * 255, (height[i] + 0.5) * 255, (height[i] + 0.5) * 255);
        map.setPixel(i / length, i % length, qRgb(gray, gray, gray));
    }

    qDebug()<<map.save("map.png");
}

void Terrain::erode()
{
    int ite, rand;
    double* v8;
    ite = rand = 0;

    /*while(ite < 10000)
    {
        for(int i = 0; i < width; i++)
        {
            for(int j = 0; j < length; j++)
            {
                if(qrand() % 100 + 1 )
                // Erosion Th
                v8 = V8(i, j);

                for(int s = 0; s < 8; s++)
                {
                    if(v8[s] != 0)
                    {

                    }
                }
            }
        }

        ite++;
    }*/
}

void Terrain::initGradTemper(){
    double z,zx1,zx0,zy1,zy0,dx,dy;
    for(int i=0;i<width;i++){
        for(int j=0;j<length;j++){
            z=getHeightAt(i,j);
            if(i+1<width&&i-1>0&&j+1<length&&j-1>0){
                zx1=getHeightAt(i+1,j);
                zx0=getHeightAt(i-1,j);
                zy1=getHeightAt(i,j+1);
                zy0= getHeightAt(i,j-1);
                dx=(zx1-zx0)/2;
                dy=(zy1-zy0)/2;
                setNormAt(i,j,QVector3D(-dx,-dy,1));
           }

        }
    }
}

void Terrain::initializeSlope()
{
    int count = 0;
    double* v8;
    double slope, temp;
    slope = temp = 0;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < length; j++)
        {
            v8 = V8(i, j);
            count = 0;
            temp = 0;

            for(int s = 0; s < 8; s++)
            {
                if(v8[s] != 0)
                {
                    temp += abs(v8[s]);
                    count++;
                }
            }

            setAvgSlope(i, j, temp / count);
        }
    }

    int gray, val;
    QImage map = QImage(width, length, QImage::Format_RGB32);
    double max = *std::max_element(height.constBegin(), height.constEnd());

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < length; j++)
        {
            val = (int)getAvgSlope(i, j) / max * 255;
            gray = qGray(val, val, val);
            map.setPixel(i, j, qRgb(gray, gray, gray));
        }
    }

    qDebug()<<map.save("SlopeMap.png");
}

void Terrain::initializeDirt()
{
    double max = *std::max_element(height.constBegin(), height.constEnd());
    double baseDirtValue = max / 10;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < length; j++)
        {
            setDirtAt(i, j, baseDirtValue);
        }
    }
}

int Terrain::getLength()
{
    return length;
}
int Terrain::getWidth()
{
    return width;
}

double* Terrain::V8(int x, int y)
{
    double* ret = new double[8];

    for(int i = 0; i < 8; i++)
    {
        ret[i] = 0;
    }

    if (x + 1 < length)
    {
        ret[0] = getHeightAt(x + 1, y);
    }

    if (x > 0)
    {
        ret[4] = getHeightAt(x - 1, y);
    }

    if (y > 0)
    {
        ret[6] = getHeightAt(x, y - 1);
    }

    if (y + 1 < width)
    {
        ret[2] = getHeightAt(x, y + 1);
    }

    if (x > 0 && y > 0)
    {
        ret[5] = getHeightAt(x - 1, y - 1);
    }

    if (x + 1 < length && y > 0)
    {
        ret[7] = getHeightAt(x + 1, y - 1);
    }

    if (y + 1 < width && x > 0)
    {
        ret[3] = getHeightAt(x - 1, y + 1);
    }

    if (y + 1 < width && x + 1 < length)
    {
        ret[1] = getHeightAt(x + 1, y + 1);
    }

    return ret;
}
