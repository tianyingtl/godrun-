#include<iostream>
#include<vector>
#include<string>
#include<easyx.h>
#include<graphics.h>
#include<conio.h>
#include<ctime>
#include<Windows.h>
#include <direct.h>
using namespace std;
const int screenWidth = 800;
const int screenHeight = 600;
#define PI       3.14159265358979323846
int gailv = 300;
double t = 1.0; // 用于控制方块生成的时间间隔
const int groundlevel = 500;//这是一个地板的y值
class Player {
public:
    double x, y;
    double width, height;
    double velocityY;
    bool isJumping;
    bool isCrouching;
    double originalHeight;


};
const double g = 5.0;
const double jump_force = -40.0;
class block {
public:
    double x, y;
    double width, height;
    bool isAirObstacle;

};
class daoju {
public:
    double x, y;
    double width, height;
    bool isActive;
};
class Game {
private:
    Player player;
    vector<block> blocks;
    vector<daoju> daojus;
    int score;
    double gamespeed;
    bool isRunning;
    double blockspeed;
    int player_img_w, player_img_h;
    IMAGE img;
    IMAGE imgbg;
    IMAGE imgza;
    IMAGE imgrw;
    IMAGE imgx;
    IMAGE imgd;
    IMAGE imgrwd;
    bool isHardMode;
    bool iswudi;
    int wuditime;


public:

    Game() {
        player.width = 50;
        player.height = 50;
        player.x = 50;
        player.y = groundlevel - player.height;
        player.velocityY = 0;
        player.isJumping = false;
        score = 0;
        gamespeed = 20.0;
        isRunning = true;
        blockspeed = 5.0;
        player.isCrouching = false;
        player.height = player.originalHeight = 50;
        isHardMode = false;
        iswudi = false;
        wuditime = 0;
    }

    void imagel() {
        // 直接用相对路径加载图片，确保图片和可执行文件在同一目录
        loadimage(&img, _T("kaishi.bmp"), screenWidth, screenHeight);
        loadimage(&imgbg, _T("diban.png"), screenWidth, 100);
        loadimage(&imgd, _T("daoju.png"), 40, 40);

        loadimage(&imgrw, _T("renwu.png"), player.width, player.height);
        imgrwd.Resize(player.height, player.width);
        // 使用 rotateimage 函数进行旋转，PI / 2.0 代表90度
        rotateimage(&imgrwd, &imgrw, PI / 2.0);
    }


    void showStartScreen() {

        putimage(0, 0, &img);
        // 设置按钮的尺寸和位置
        int buttonWidth = 200;
        int buttonHeight = 60;
        int buttonX = (screenWidth - buttonWidth) / 2;
        int buttonY = screenHeight - 200; // 第一个按钮位置上移
        int button2Y = screenHeight - 120; // 第二个按钮位置

        // 绘制第一个按钮背景 (普通模式)
        setfillcolor(RGB(50, 200, 50)); // 绿色按钮
        setlinecolor(WHITE);
        setlinestyle(PS_SOLID, 3);
        fillroundrect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight, 15, 15);

        // 绘制第二个按钮背景 (困难模式)
        setfillcolor(RGB(200, 50, 50)); // 红色按钮
        fillroundrect(buttonX, button2Y, buttonX + buttonWidth, button2Y + buttonHeight, 15, 15);

        // 设置文字样式
        settextcolor(WHITE);
        setbkmode(TRANSPARENT);
        LOGFONT f;
        gettextstyle(&f);
        f.lfHeight = 30;
        f.lfWeight = FW_BOLD;
        _tcscpy_s(f.lfFaceName, _T("微软雅黑"));
        settextstyle(&f);

        // 绘制第一个按钮文字
        TCHAR buttonText[] = _T("普通模式");
        int textWidth = textwidth(buttonText);
        int textHeight = textheight(buttonText);
        outtextxy(buttonX + (buttonWidth - textWidth) / 2,
            buttonY + (buttonHeight - textHeight) / 2,
            buttonText);

        // 绘制第二个按钮文字
        TCHAR button2Text[] = _T("困难模式");
        textWidth = textwidth(button2Text);
        outtextxy(buttonX + (buttonWidth - textWidth) / 2,
            button2Y + (buttonHeight - textHeight) / 2,
            button2Text);
        FlushBatchDraw();
        // 等待鼠标点击按钮
        while (true) {
            ExMessage msg = getmessage(EM_MOUSE);
            if (msg.message == WM_LBUTTONDOWN) {
                // 检查是否点击普通模式按钮
                if (msg.x >= buttonX && msg.x <= buttonX + buttonWidth &&
                    msg.y >= buttonY && msg.y <= buttonY + buttonHeight) {
                    // 按钮点击效果
                    setfillcolor(RGB(30, 150, 30)); // 按下时颜色变暗
                    fillroundrect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight, 15, 15);
                    outtextxy(buttonX + (buttonWidth - textWidth) / 2,
                        buttonY + (buttonHeight - textHeight) / 2,
                        buttonText);
                    FlushBatchDraw();
                    Sleep(100); // 短暂延迟，产生按下效果
                    isHardMode = false; // 设置为普通模式
                    break; // 点击了按钮，退出循环开始游戏
                }

                // 检查是否点击困难模式按钮
                if (msg.x >= buttonX && msg.x <= buttonX + buttonWidth &&
                    msg.y >= button2Y && msg.y <= button2Y + buttonHeight) {
                    // 按钮点击效果
                    setfillcolor(RGB(150, 30, 30)); // 按下时颜色变暗
                    fillroundrect(buttonX, button2Y, buttonX + buttonWidth, button2Y + buttonHeight, 15, 15);
                    outtextxy(buttonX + (buttonWidth - textWidth) / 2,
                        button2Y + (buttonHeight - textHeight) / 2,
                        button2Text);
                    FlushBatchDraw();
                    Sleep(100); // 短暂延迟，产生按下效果
                    isHardMode = true; // 设置为困难模式
                    break; // 点击了按钮，退出循环开始游戏
                }
            }
        }
    }
    void resetgame() {
        player.y = groundlevel - player.height;
        player.velocityY = 0;
        player.isJumping = false;
        score = 0;
        if (isHardMode) {
            gamespeed = 40.0;  // 困难模式下初始速度更快
            t = 2;
            gailv = 600;// 困难模式下道具更难生成
        }
        else {
            gamespeed = 20.0;  // 普通模式初始速度
            t = 1.0;// 普通模式方块生成间隔
            gailv = 300;
        }
        isRunning = true;
        blockspeed = 5.0;
        blocks.clear(); // 清空上一局的障碍物
    }
    void run() {
        initgraph(screenWidth, screenHeight);
        imagel();
        BeginBatchDraw();
        showStartScreen();
        while (true) {
            resetgame();

            while (isRunning) {
                handleinput();
                update();
                draw();
                FlushBatchDraw();
                Sleep(16); // 控制帧率
            }


            int choice = showgameover();

            if (choice == 0) {
                break; // 选择退出游戏
            }
        }
        EndBatchDraw();
        closegraph();

    }
private:
    void handleinput() {
        // 使用 GetAsyncKeyState 来检测按键，它在游戏循环中更可靠。。
        // & 0x8000 用来检查该键当前是否正被按下。
        if (GetAsyncKeyState(0x20) & 0x8000) {
            if (!player.isJumping) {
                player.isJumping = true;
                player.velocityY = jump_force;
                player.isCrouching = false;
            }
        }
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
            if (!player.isJumping) { // 不在空中时才能蹲下
                player.isCrouching = true;
            }
        }
        else {
            // 如果之前是蹲下状态，现在松开按键了，就恢复站立
            if (player.isCrouching) {
                player.isCrouching = false;
            }
        }
    }

    void update() {
        if (player.isJumping) {
            player.y += player.velocityY;
            player.velocityY += g; // 重力加速度
        }
        if (player.y >= groundlevel - player.height) {
            player.y = groundlevel - player.height;
            player.isJumping = false;
            player.velocityY = 0;
        }//检测是否落地
        // 检测蹲下状态
        if (player.isCrouching) {
            player.height = 25; // 蹲下时的高度
        }
        else {

            player.height = player.originalHeight; // 恢复原始高度
        }
        // 更新方块位置
        for (auto& b : blocks) {
            b.x = b.x - gamespeed; // 根据游戏速度更新方块位置
        }

        // 删除超出屏幕的方块
        for (auto it = blocks.begin(); it != blocks.end();) {
            if (it->x + it->width < 0) {
                it = blocks.erase(it);
            }
            else {
                ++it;
            }
        }
        // 添加新的方块
        blockspeed += t;
        if (blockspeed > 40 + rand() % 40) { // 随机间隔
            blockspeed = 0;
            block newObs;
            newObs.width = 30 + rand() % 30;

            // 随机决定是地面障碍物还是空中障碍物
            bool isAir = (rand() % 3 == 0); // 约1/3概率生成空中障碍物
            newObs.isAirObstacle = isAir;

            if (isAir) {
                double playerHeadHeight = groundlevel - player.originalHeight;
                double playerCrouchHeight = groundlevel - 25;

                // 障碍物的底部边缘应该在这两个高度之间
                double obstacleBottom = playerHeadHeight + (player.originalHeight - 25) / 2;

                newObs.height = obstacleBottom; // 从顶部(0)到计算的底部高度
                newObs.width = 100 + rand() % 100; // 宽度可以稍窄一些
                newObs.x = screenWidth;
                newObs.y = 0; // 从屏幕顶部开始
            }
            else {
                // 地面障碍物 - 较高需要跳跃
                newObs.height = 50 + rand() % 100;
                newObs.x = screenWidth;
                newObs.y = groundlevel - newObs.height;
            }

            blocks.push_back(newObs);
        }

        for (auto it = daojus.begin(); it != daojus.end();) {
            if (player.x < it->x + it->width && player.x + player.width > it->x &&
                player.y < it->y + it->height && player.y + player.height > it->y) {
                // 碰到道具，激活无敌状态
                iswudi = true;
                wuditime = 180; // 持续时间约3秒(180帧)

                it = daojus.erase(it); // 移除已获取的道具
            }
            else {
                ++it;
            }
        }

        // 更新无敌状态
        if (iswudi) {
            wuditime--;
            if (wuditime <= 0) {
                iswudi = false;
            }
        }
        // 检测碰撞
        for (const auto& b : blocks) {
            if (iswudi) {
                continue;
            }
            if (player.x < b.x + b.width && player.x + player.width > b.x) {
                // 对于空中障碍物，只有当玩家站立且没蹲下时才检测碰撞
                if (b.isAirObstacle) {
                    // 只检测头部区域的碰撞
                    double headTop = player.y;
                    double headBottom = player.y + player.originalHeight / 2;

                    if (!player.isCrouching &&
                        headBottom > b.y &&
                        headTop < b.y + b.height) {
                        isRunning = false; // 发生碰撞，游戏结束
                    }
                }
                // 对于地面障碍物，正常检测碰撞
                else if (player.y < b.y + b.height &&
                    player.y + player.height > b.y) {
                    isRunning = false; // 发生碰撞，游戏结束
                }
            }
        }
        if (rand() % gailv == 0) { // 概率生成道具，较为稀有
            daoju newPowerUp;
            newPowerUp.width = 40;
            newPowerUp.height = 40;
            newPowerUp.x = screenWidth;
            newPowerUp.y = groundlevel - player.originalHeight - 50 - rand() % 100; // 在玩家可以跳到的高度
            newPowerUp.isActive = true;

            daojus.push_back(newPowerUp);
        }

        // 更新道具位置
        for (auto& p : daojus) {
            p.x = p.x - gamespeed; // 与障碍物同速移动
        }

        // 删除超出屏幕的道具
        for (auto it = daojus.begin(); it != daojus.end();) {
            if (it->x + it->width < 0) {
                it = daojus.erase(it);
            }
            else {
                ++it;
            }
        }
        //更新分数和游戏速度
        score += 2; // 每帧增加分数
        if (score % 500 == 0 && gamespeed < 60) { // 每500分增加游戏速度
            gamespeed += 1.0;
            t += 0.3; // 增加方块生成的时间间隔

        }

    }
    void draw() {
        cleardevice();
        // 绘制地面
        putimage(0, groundlevel, &imgbg);
        // 绘制玩家
        if (player.isCrouching) {
            // 绘制旋转后的“躺下”图像
                   // 旋转后，图像的视觉高度变为原始宽度，所以y坐标需要相应调整
            double drawY = groundlevel - player.width;
            putimage(player.x, drawY, &imgrwd);
        }
        else {
            // 站立时，显示完整图像
            putimage(player.x, player.y, player.width, player.height, &imgrw, 0, 0);
        }
        // 绘制障碍物
        for (const auto& obs : blocks) {
            if (obs.isAirObstacle) {
                loadimage(&imgx, _T("xia.png"), obs.width, obs.height);
                putimage(obs.x, obs.y, obs.width, obs.height, &imgx, 0, 0);
            }
            else {
                // 地面障碍物
                loadimage(&imgza, _T("zhangai.png"), obs.width, obs.height);
                putimage(obs.x, obs.y, obs.width, obs.height, &imgza, 0, 0);
            }
        }
        // 绘制道具
        for (const auto& p : daojus) {

            putimage(p.x, p.y, p.width, p.height, &imgd, 0, 0);
        }
        // 如果玩家处于无敌状态，添加视觉效果
        if (iswudi) {
            // 绘制一个金色光环
            setlinecolor(RGB(255, 215, 0)); // 金色
            setlinestyle(PS_SOLID, 3);
            circle(player.x + player.width / 2, player.y + player.height / 2, player.width * 0.7);

            // 添加无敌状态提示文字
            TCHAR invText[32];
            _stprintf_s(invText, _T("无敌状态: %.1f秒"), wuditime / 60.0);
            outtextxy(10, 160, invText);
        }

        // 绘制分数
        settextcolor(WHITE);
        setbkmode(TRANSPARENT);
        LOGFONT f;
        gettextstyle(&f);
        f.lfHeight = 30;
        _tcscpy_s(f.lfFaceName, _T("Arial"));
        settextstyle(&f);

        TCHAR scoreText[32];
        TCHAR scoreT[32];
        TCHAR sc2[32];
        TCHAR sc3[32];
        TCHAR modeText[32];
        if (isHardMode) {
            _stprintf_s(modeText, _T("困难模式"));
            settextcolor(RGB(255, 100, 100)); // 红色显示困难模式
        }
        else {
            _stprintf_s(modeText, _T("普通模式"));
            settextcolor(RGB(100, 255, 100)); // 绿色显示普通模式
        }
        outtextxy(10, 130, modeText);
        settextcolor(WHITE); // 恢复默认文字颜色
        _stprintf_s(scoreText, _T("Score: %d"), score);
        _stprintf_s(scoreT, _T("游戏速度: %.1f"), gamespeed);
        _stprintf_s(sc2, _T("方块速度: %.1f"), t);
        _stprintf_s(sc3, _T("按空格跳跃 Ctrl蹲下"));

        outtextxy(10, 10, scoreText);
        outtextxy(10, 40, scoreT);
        outtextxy(10, 70, sc2);
        outtextxy(10, 100, sc3);
    }

    int showgameover() {
        cleardevice();
        LOGFONT f;
        gettextstyle(&f);
        f.lfHeight = 60;
        _tcscpy_s(f.lfFaceName, _T("微软雅黑"));
        settextstyle(&f);
        settextcolor(LIGHTRED);

        TCHAR gameOverText[] = _T("游戏结束");
        int textWidth = textwidth(gameOverText);
        outtextxy((screenWidth - textWidth) / 2, screenHeight / 2 - 120, gameOverText);

        f.lfHeight = 30;
        settextstyle(&f);
        TCHAR finalScoreText[32];
        _stprintf_s(finalScoreText, _T("最终分数: %d"), score);
        textWidth = textwidth(finalScoreText);
        outtextxy((screenWidth - textWidth) / 2, screenHeight / 2 - 50, finalScoreText);

        // 设置按钮的尺寸和位置
        int buttonWidth = 200;
        int buttonHeight = 50;
        int buttonX = (screenWidth - buttonWidth) / 2;
        int button1Y = screenHeight / 2;        // 普通模式按钮
        int button2Y = screenHeight / 2 + 60;   // 困难模式按钮
        int button3Y = screenHeight / 2 + 120;  // 退出游戏按钮

        // 绘制三个按钮
        // 普通模式按钮（绿色）
        setfillcolor(RGB(50, 200, 50));
        setlinecolor(WHITE);
        setlinestyle(PS_SOLID, 3);
        fillroundrect(buttonX, button1Y, buttonX + buttonWidth, button1Y + buttonHeight, 15, 15);

        // 困难模式按钮（红色）
        setfillcolor(RGB(200, 50, 50));
        fillroundrect(buttonX, button2Y, buttonX + buttonWidth, button2Y + buttonHeight, 15, 15);

        // 退出游戏按钮（灰色）
        setfillcolor(RGB(100, 100, 100));
        fillroundrect(buttonX, button3Y, buttonX + buttonWidth, button3Y + buttonHeight, 15, 15);

        // 设置文字样式
        settextcolor(WHITE);
        setbkmode(TRANSPARENT);
        f.lfHeight = 25;
        f.lfWeight = FW_BOLD;
        settextstyle(&f);

        // 绘制按钮文字
        TCHAR button1Text[] = _T("重新开始普通模式");
        TCHAR button2Text[] = _T("重新开始困难模式");
        TCHAR button3Text[] = _T("结束游戏");

        textWidth = textwidth(button1Text);
        int textHeight = textheight(button1Text);
        outtextxy(buttonX + (buttonWidth - textWidth) / 2, button1Y + (buttonHeight - textHeight) / 2, button1Text);

        textWidth = textwidth(button2Text);
        outtextxy(buttonX + (buttonWidth - textWidth) / 2, button2Y + (buttonHeight - textHeight) / 2, button2Text);

        textWidth = textwidth(button3Text);
        outtextxy(buttonX + (buttonWidth - textWidth) / 2, button3Y + (buttonHeight - textHeight) / 2, button3Text);

        FlushBatchDraw();

        // 等待鼠标点击按钮
        while (true) {
            ExMessage msg = getmessage(EM_MOUSE);
            if (msg.message == WM_LBUTTONDOWN) {
                // 检查点击的是哪个按钮
                if (msg.x >= buttonX && msg.x <= buttonX + buttonWidth) {
                    // 普通模式按钮
                    if (msg.y >= button1Y && msg.y <= button1Y + buttonHeight) {
                        // 按钮点击效果
                        setfillcolor(RGB(30, 150, 30));
                        fillroundrect(buttonX, button1Y, buttonX + buttonWidth, button1Y + buttonHeight, 15, 15);
                        outtextxy(buttonX + (buttonWidth - textWidth) / 2, button1Y + (buttonHeight - textHeight) / 2, button1Text);
                        FlushBatchDraw();
                        Sleep(100);
                        isHardMode = false;
                        return 1; // 普通模式
                    }
                    // 困难模式按钮
                    else if (msg.y >= button2Y && msg.y <= button2Y + buttonHeight) {
                        setfillcolor(RGB(150, 30, 30));
                        fillroundrect(buttonX, button2Y, buttonX + buttonWidth, button2Y + buttonHeight, 15, 15);
                        outtextxy(buttonX + (buttonWidth - textWidth) / 2, button2Y + (buttonHeight - textHeight) / 2, button2Text);
                        FlushBatchDraw();
                        Sleep(100);
                        isHardMode = true;
                        return 2; // 困难模式
                    }
                    // 退出游戏按钮
                    else if (msg.y >= button3Y && msg.y <= button3Y + buttonHeight) {
                        setfillcolor(RGB(70, 70, 70));
                        fillroundrect(buttonX, button3Y, buttonX + buttonWidth, button3Y + buttonHeight, 15, 15);
                        outtextxy(buttonX + (buttonWidth - textWidth) / 2, button3Y + (buttonHeight - textHeight) / 2, button3Text);
                        FlushBatchDraw();
                        Sleep(100);
                        return 0; // 退出游戏
                    }
                }
            }
        }
    }

};
int main() {

    srand(static_cast<unsigned int>(time(0))); // 初始化随机数种子
    Game game;
    game.run();
    return 0;
}



