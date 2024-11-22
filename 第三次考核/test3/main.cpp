#include <iostream>
#include <stdio.h>
#include <opencv4/opencv2/opencv.hpp>
using namespace std;

class Light
{
    public:
    double width;
    double length;
    double ratio;
    double angle;
    double rect;
    cv::Point2f top;
    cv::Point2f bottom;

};

bool isLight(const Light & light)
{
    float ratio = light.width / light. length;
    double min_ratio = 0.01;
    double max_ratio = 0.70;
    bool ratio_ok = min_ratio < ratio && ratio < max_ratio;
    double max_angle = 30;
    bool angle_ok = light.angle < max_angle ;
    bool is_light = ratio_ok && angle_ok;
    return is_light;
}

//提取轮廓
vector<Light> findLights( const cv::Mat img1, cv::Mat & img4)
{
    cout<<"41\n";
    vector<vector<cv::Point>> contours;
    cout<<"42\n";
    vector<cv::Vec4i> hierarchy;
    cout<<"43\n";
    cv::findContours(img4, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    cout<<"44\n";
    vector<Light> lights;
    cout<<"45\n";
    //拟合最小面积外接矩形
    for (const auto & contour : contours) {


        cout << "45.1\n";
        cv::RotatedRect r_rect = cv::minAreaRect(contour);

        cout << "45.2\n";
        Light light;
        cout << "45.3\n";
        light.ratio = r_rect.size.width / r_rect.size.height;
        cout << "45.4\n";
        light.width = r_rect.size.width;
        light.length = r_rect.size.height;
        if (r_rect.angle > 90) {
            r_rect.angle = r_rect.angle-90;
            if (r_rect.angle > 180) {
                r_rect.angle = r_rect.angle - 90;
                if (r_rect.angle > 270) {
                    r_rect.angle = r_rect.angle - 90;
                    }
                }
            }
        light.angle = abs(r_rect.angle);

        cv::Point2f p[4];
        r_rect.points(p);
        //排序
        sort(p, p + 4, [](const cv::Point2f &a, const cv::Point2f &b) { return a.x < b.x; });
        light.top = (p[0] + p[2]) / 2;
        light.bottom = (p[1] + p[3]) / 2;
        cout << "45.6\n";
        if (isLight(light)) {
            cout << "45.5\n";
            lights.push_back(light);
            for(int i=0; i<contours.size(); i++) {
//                cv::RotatedRect box[i];
//                cv::Rect boundRect[i];
//                cv::Point2f rect[4];
//                box[i] = minAreaRect(cv::Mat(contours[i]));  //计算每个轮廓最小外接矩形
//                boundRect[i] = boundingRect(cv::Mat(contours[i]));
//                circle(img1, cv::Point(box[i].center.x, box[i].center.y), 5, cv::Scalar(0, 255, 0), -1,
//                       8);  //绘制最小外接矩形的中心点
//                box[i].points(rect);  //把最小外接矩形四个端点复制给rect数组
//                rectangle(img1, cv::Point(boundRect[i].x, boundRect[i].y),
//                          cv::Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height),
//                          cv::Scalar(0, 255, 0), 2, 8);
//                for (int j = 0; j < 4; j++) {
//                    line(img1, rect[j], rect[(j + 1) % 4], cv::Scalar(0, 0, 255), 2, 8);  //绘制最小外接矩形每条边
//                }
            }
            }
        }


    cout<<"46\n";
    return lights;
}


//装甲板类
class Armor
{
public:
    Armor(const Light & left, const Light & right) : left_light(left), right_light(right){}

    const Light &left_light;
    const Light &right_light;
    cv::Point2f position[4] = {};
    int i = 0;
};

//灯条匹配
vector<Armor> matchLights(const vector<Light> & lights)
{
    vector<Armor> armors;
    for (auto light_1 = lights.begin(); light_1 != lights.end(); light_1++)
    {
        cout<<"51\n";
        for (auto light_2 = light_1+1; light_2 != lights.end(); light_2++)
        {
            cout<<"52\n";
            if (abs(light_1->angle - light_2->angle)< 70
            && abs(light_1->width- light_2->width)< 10
            && abs(light_1->length - light_2->length) < 10
            )
            {
                cout<<"53\n";
                Armor armor(*light_1, *light_2);
                cout<<"54\n";

                armor.position[0] = light_1->top;
                armor.position[1] = light_1->bottom;
                armor.position[2] = light_2->top;
                armor.position[3] = light_2->bottom;
                cout<<"57\n";
                cout<<"armor.position[0]"<<armor.position[0]<<endl;
                cout<<"armor.position[1]"<<armor.position[1]<<endl;
                cout<<"armor.position[2]"<<armor.position[2]<<endl;
                cout<<"armor.position[3]"<<armor.position[3]<<endl;
                armors.push_back(armor);
                armor.i++;
            }
        }
    }
    return armors ;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    //输入img1,原图
    cv::Mat img1 = cv::imread( argv[1], cv::IMREAD_COLOR);
    cout<<"1\n";

    //通道相减法
    vector<cv::Mat> channels;
    cv::split(img1,channels);
    //gray为通道相减后，img2为二值化后
    cv::Mat gray, img2;
    cv::subtract(channels[2], channels[0], gray);
    //二值化
    cv::threshold(gray, img2, 90, 255, cv::THRESH_BINARY);
    cout<<"2\n";

    //闭运算
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5));
    //输入img3,闭运算后图
    cv::Mat img3;
    cv::morphologyEx(img2, img3, cv::MORPH_CLOSE, kernel, cv::Point(-1,-1),4);

    //canny边缘检测
    //输入img4，轮廓检测后图
    cv::Mat img4;
    cv::Canny(img3, img4, 100, 200);
    cout<<"3\n";

    // 检测灯条
    vector<Light> lights = findLights(img1,img4);
    cout<<"4\n";



    // 匹配灯条并获取装甲板信息
    vector<Armor> armors = matchLights(lights);
    int n = armors.size();
    cout<<"n="<<n<<endl;
    cout<<"5\n";

    //绘制线段
    for (int j=0 ; j <= n ; j++)
    if (!armors.empty()) {
        cv::line(img1, armors[j].position[0], armors[j].position[3], cv::Scalar(255, 0, 0),
                 2, 8);
        cv::line(img1, armors[j].position[1], armors[j].position[2], cv::Scalar(255, 0, 0),
                 2, 8);
        cout<<"6\n";
    }
    else{
        cout<<"No Armor Detected!\n";
    }
    cv::imshow("Detected Armor", img1);
    cv::waitKey(0);

//    //显示img
//    cv::resize(img1,img1, cv::Size(640,512));
//    cv::namedWindow("img1", cv::WINDOW_AUTOSIZE );
//    cv::imshow("img1", img1);
//
//    //显示gray
//    cv::resize(gray,gray, cv::Size(640,512));
//    cv::imshow("gray", gray);
//
//    //显示img2
//    cv::resize(img2,img2, cv::Size(640,512));
//    cv::imshow("img2", img2);
//
//    //显示img3
//    cv::resize(img3,img3, cv::Size(640,512));
//    cv::imshow("img3", img3);

    //显示img4
    cv::resize(img4,img4, cv::Size(640,512));
    cv::imshow("img4", img4);
    cout<<"7\n";

    cv::waitKey(0);
    return 0;
}
