//第二次考核  弹道解算
#include <iostream>
#include <cmath>
#include <functional>
using namespace std;

const double v0=17.0;//初速度为17m/s
const double g=9.8;//重力加速度为9.8m/s^2
const double x0=3.0, y=4.0, z0=0.25;//目标坐标
const double s0=sqrt(x0*x0+y*y);//水平距离
const double C=0.47;//球体在空气中的摩擦系数为0.47 
const double rho=1.293;//空气密度为1.293kg/m^3
const double r=16.8e-3;//球体半径为16.8mm
const double m=3.2e-3;//球体质量为3.2g
const double S=M_PI*r*r;
const double pi=M_PI;
const double k0=C*rho*S/2;
const double k1=k0/m;



double s_motion(double theta,double t)
{
    double vs= v0*cos(theta);
    double vx0=1/C;
    return 1/k1*log(k1*vx0*t+1);//返回水平面距离
}

//初始化
//const double t=0.01;
double z_actual (double theta, double t)
{
    double tx=exp(k1-1)/(k1*v0*cos(theta));
    return v0*t*sin(theta)-0.5*g*t*t;//返回垂直面高度
}


pair<double,double>angle()
{
    double theta=pi/11;//初始角度猜测
    //初始值为pi/4时，theta为157.177；pi/6,theta157.164;pi/8,theta147.49;pi/12,theta66.8291
    //pi/10,theta65.672,pi/11,theta93.4016
    double t0=1.0;//初始时间猜测
    double tolerance=0.0001;//误差容限
    int max_iterations=20;//最大迭代次数

    for(int i=0; i<max_iterations; i++)//计算误差并迭代
    {
        double s_pos=s_motion(theta,t0);//实际水平面射点
        double z_pos=z_actual(theta,t0);//实际数值面射点

        double ds=s0-s_pos;//计算水平面误差
        double dz=-z_pos;//计算垂直面误差

        if (fabs(ds)<tolerance && fabs(dz)<tolerance)
            break;//满足误差要求则退出迭代

        //更新枪口
        double z_temp=z0;//z_temp初始值为z0
        double s_temp=s0;//s_temp初始值为s0
        z_temp=z_temp+dz;//迭代z_temp
        s_temp=s_temp+ds;//迭代s_temp
        theta=z_temp/s_temp;//更新theta
    }
    return{theta, t0};//返回角度theta和时间t
   
}

int main()
{
    auto[theta, t0]=angle();
    theta=theta*180/pi;//转换为角度
    cout<<"炮台的初射角度为："<<theta<<"度";
    return 0;

}