#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <opencv2\opencv.hpp>

class myPoint
{
public:
	int x;
	int y;
	int dxl1;
	int dxl2;
	int d;
};

class planToolBox
{
public:
	planToolBox();
	~planToolBox();
	bool getPosition(int x, int y);              //查表，计算（x,y）对应的舵机位置
	void getPoints();                            //加载标定表中的数据
	bool setDataPath(std::string data_path);     //设置标定表路径
	void getDXLPosition(int &dxl1, int &dxl2);   //获取舵机位置

private:
	bool triVal(int x, int y);                                //三角平滑处理
	bool isInTriangle(myPoint p1, myPoint p2, myPoint p3);    //判断目标点是否在三角形p1p2p3内部
	void smoothing(myPoint p1, myPoint p2, myPoint p3);       //二维映射三维，计算舵机位置
	static bool sortFun(myPoint p1, myPoint p2);              //按照标定表中点距离目标点的距离排序
	myPoint p;
	std::vector<myPoint> points;
	std::ifstream in;
};

planToolBox::planToolBox()
{
	p = myPoint();
}

planToolBox::~planToolBox() {}

bool planToolBox::getPosition(int x, int y)
{
	p.x = x;
	p.y = y;
	p.d = 0;
	for (int i = 0; i < points.size(); i++)
	{
		if (points[i].x == x && points[i].y == y)
		{
			p.dxl1 = points[i].dxl1;
			p.dxl2 = points[i].dxl2;
			return true;
		}
		points[i].d = (points[i].x - p.x) * (points[i].x - p.x) + (points[i].y - p.y) * (points[i].y - p.y);
	}
	//printf("%d\n", points[0].d);
	return triVal(x, y);
}

void planToolBox::getPoints()
{
	std::string filename, angle1, angle2, x, y;
	myPoint tmp = myPoint();
	while (!in.eof())
	{
		//in >> filename;
		in >> angle1;
		in >> angle2;
		in >> x;
		in >> y;
		if (in.fail())
		{
			break;
		}
		tmp.x = stoi(x);
		tmp.y = stoi(y);
		tmp.dxl1 = stoi(angle1);
		tmp.dxl2 = stoi(angle2);
		//tmp.d = (tmp.x - p.x) * (tmp.x - p.x) + (tmp.y - p.y) * (tmp.y - p.y);
		points.push_back(tmp);
	}
	in.close();
}

bool planToolBox::setDataPath(std::string path)
{
	in.open(path);
	return in.is_open();
}

bool planToolBox::triVal(int x, int y)
{
	std::vector<myPoint> tmp = points;
	sort(tmp.begin(), tmp.end(), this->sortFun);

	for (int i = 0; i < tmp.size() - 2; i++)
	{
		for (int j = i + 1; j < tmp.size() - 1; j++)
		{
			for (int k = j + 1; k < tmp.size(); k++)
			{
				if (isInTriangle(tmp[i], tmp[j], tmp[k]))
				{
					smoothing(tmp[i], tmp[j], tmp[k]);
					return true;
				}
			}
		}
	}
	printf("can not find triangle!\n");
	return false;
}

bool planToolBox::sortFun(myPoint p1, myPoint p2)
{
	return p1.d < p2.d;
}

bool planToolBox::isInTriangle(myPoint p1, myPoint p2, myPoint p3)
{
	int a = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
	int b = -((p3.x - p1.x) * (p.y - p1.y) - (p.x - p1.x) * (p3.y - p1.y));
	int c = (p2.x - p1.x) * (p.y - p1.y) - (p.x - p1.x) * (p2.y - p1.y);
	if (a == 0)
	{
		return false;
	}
	int u = b / a;
	int v = c / a;
	return u >= 0 && v >= 0 && u + v <= 1;
}

void planToolBox::smoothing(myPoint p1, myPoint p2, myPoint p3)
{
	int a1 = (p2.y - p1.y) * (p3.dxl1 - p1.dxl1) - (p3.y - p1.y) * (p2.dxl1 - p1.dxl1);
	int b1 = -((p2.x - p1.x) * (p3.dxl1 - p1.dxl1) - (p3.x - p1.x) * (p2.dxl1 - p1.dxl1));
	int c1 = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
	p.dxl1 = -(a1 * (p.x - p1.x) + b1 * (p.y - p1.y) - c1 * p1.dxl1) / c1;

	int a2 = (p2.y - p1.y) * (p3.dxl2 - p1.dxl2) - (p3.y - p1.y) * (p2.dxl2 - p1.dxl2);
	int b2 = -((p2.x - p1.x) * (p3.dxl2 - p1.dxl2) - (p3.x - p1.x) * (p2.dxl2 - p1.dxl2));
	int c2 = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
	p.dxl2 = -(a2 * (p.x - p1.x) + b2 * (p.y - p1.y) - c2 * p1.dxl2) / c2;
}

void planToolBox::getDXLPosition(int &dxl1, int &dxl2)
{
	dxl1 = p.dxl1;
	dxl2 = p.dxl2;
}