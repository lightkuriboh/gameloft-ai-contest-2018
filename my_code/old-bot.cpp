#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <random>

using namespace std;

typedef pair<int, int> vec2;

#define X first
#define Y second
#define mp make_pair

#define sqr(x) ((x) * (x))

#define MOVE 1
#define SHOOT 2
#define WAIT 0

int myTeamID = 0;
int MAP_W = 14400;
int MAP_H = 9600;
int numTurn = 1000;

const vec2 theNetY = vec2(3000, 5200);
const int trailingNet = 1500;
const int distKickX = 3500;

const int maxMove = 300;
const int maxKick = 200;
const int deaccelation = 120;

class Ball {
public:
    vec2 ballPosition;
    vec2 ballSpeed;
    vector <vec2> possible, speed;

    Ball (const int &x, const int &y) {
        this->ballPosition = mp(x, y);
    }

    void updateBallInfo (const int &x, const int &y, const int &sx, const int &sy) {
        this->ballPosition = mp(x, y);
        this->ballSpeed = mp(sx, sy);
        this->predictBall();
    }

    int speedCalc() {
        return sqrt(sqr(this->ballSpeed.X) + sqr(this->ballSpeed.Y));
    }

    void predictBall() {
        possible.clear();
        speed.clear();
        int speedNow = this->speedCalc();
        possible.push_back(this->ballPosition);

        vec2 curPosition = this->ballPosition;
        vec2 curSpeed = this->ballSpeed;

        while (speedNow >= deaccelation) {
            curPosition.X += curSpeed.X;
            curPosition.Y += curSpeed.Y;

            if (curPosition.X > MAP_W || curPosition.Y > MAP_H || curPosition.X < 0 || curPosition.Y < 0) break;

            possible.push_back(curPosition);

            speedNow -= deaccelation;

            if (this->ballSpeed.Y != 0) {

                double tmp = 1.0 + double(sqr(this->ballSpeed.X)) / double(sqr(this->ballSpeed.Y));
                int newSpeedY = sqrt(double(sqr(speedNow)) / tmp);
                if (curSpeed.Y != 0) {
                    newSpeedY *= curSpeed.Y / abs(curSpeed.Y);
                }

                int newSpeedX = sqrt(sqr(speedNow) - sqr(newSpeedY));
                if (curSpeed.X != 0) {
                     newSpeedX *= curSpeed.X / abs(curSpeed.X);
                }

                //cout << newSpeedX << " " << newSpeedY <<"saddas\n";

                curSpeed = mp(newSpeedX, newSpeedY );
            } else {
                if (curSpeed.X != 0) {
                    curSpeed = mp(speedNow * curSpeed.X / abs(curSpeed.X), 0);
                }
                else {
                    curSpeed = mp(speedNow, 0);
                }
            }
            speed.push_back(curSpeed);
        }

        cout << this->possible.size() << "\n";
        for (auto vec: this->possible) {
            cout << vec.X << " " << vec.Y << "\n";
        }

    }

    bool canBeShoot() {
        return this->ballPosition.X >= MAP_W - distKickX &&
                theNetY.X - trailingNet <= this->ballPosition.Y && this->ballPosition.Y <= theNetY.Y + trailingNet;
    }
};

class Player {
public:
    vec2 playerPosition;
    bool isEnemy;
    bool isPaired;

    struct Action {
        int action;
        int desX, desY, force;
    } playerAction;


    Player (const int &x, const int &y, const bool &_isEnemy) {
        this->playerPosition = mp(x, y);
        this->isEnemy = _isEnemy;
        this->playerAction = Action{WAIT, 0, 0, 0};
    }

    void updatePlayerPosition(const int &x, const int &y) {
        this->playerPosition = mp(x, y);
        this->isPaired = false;
        this->catchTheBall = -1;
    }

    void updateAction(const int &action, const int &desX, const int &desY, const int &force) {
        this->playerAction.action = action;
        this->playerAction.desX = desX;
        this->playerAction.desY = desY;
        this->playerAction.force = force;
    }

    int catchTheBall;
    int distToBall(Ball *ball) {
        return sqrt(
            sqr(this->playerPosition.X - ball->ballPosition.X)
            + sqr(this->playerPosition.Y - ball->ballPosition.Y)
        );
    }
};

vector <vec2> initiatePosition =
                                {
                                    mp(4000, 2400)
                                    ,
                                    mp(4000, 3600)
                                    ,
                                    mp(4000, 4800)
                                    ,
                                    mp(4000, 6000)
                                    ,
                                    mp(4000, 7200)
                                };

class gameMatch {
public:
    Ball* ball;
    Player* player[10];

    gameMatch () {
        ball = new Ball(MAP_W / 2, MAP_H / 2);
        for (int i = 0; i < 5; i++) {
            player[i] = new Player(initiatePosition[i].X, initiatePosition[i].Y, false);
        }
        for (int i = 5; i < 10; i++) {
            player[i] = new Player(0, 0, true);
        }
    }

    int Dist(vec2 a, vec2 b) {
        return sqrt(sqr(a.X - b.X) + sqr(a.Y - b.Y));
    }

    bool weCanKickBall (Player* attacker) {

        if (attacker->distToBall(this->ball) < maxKick) {
            return true;
        }
        return false;
    }

    Player* getOurAttacker () {
        for (int i = 0; i < 5; i++) {
            if (this->weCanKickBall(this->player[i])) {
                return this->player[i];
            }
        }
        Player* ans = nullptr;

        int mnStep = 1000000000;
        int good = -1;
        for (int i = 0; i < 5; i++) {
            int mnNow = 1000000000;
            int tmpGood = -1;
            //cout << i << "'th player\n";
            for (int j = 0; j < (int)this->ball->possible.size(); j++) {
                auto pos = this->ball->possible[j];
                int distPlayerToPos = this->Dist(this->player[i]->playerPosition, pos);
                int stepToBall = distPlayerToPos / maxMove;
                int gap = j - stepToBall;
                //cout << gap << " đá\n";
                //cout << pos.X << " " << pos.Y << "/" <<this->player[i]->playerPosition.X << " " << this->player[i]->playerPosition.Y << "/" <<distPlayerToPos << " " << gap << " " << j << " dsad\n";
                if (gap > 0) {
                    if (gap < mnNow) {
                        mnNow = gap;
                        tmpGood = j;
                    }
                    if (distPlayerToPos < maxKick) {
                        ans = this->player[i];
                        ans->catchTheBall = j;
                        return ans;
                    }
                }
            }
            if (mnNow > 0 && mnNow < mnStep) {
                mnStep = mnNow;
                ans = this->player[i];
                //cout << i << " " << tmpGood << " fuck\n";
                good = tmpGood;
            }
        }

        if (ans == nullptr) {
            int minDist = 1000000000;
            for (int i = 0; i < 5; i++) {
                int mnn = 1000000000;
                for (auto pos: this->ball->possible) {
                    mnn = min(mnn, this->Dist(this->player[i]->playerPosition, pos));
                }
                if (mnn < minDist) {
                    minDist = mnn;
                    ans = this->player[i];
                }
            }
        }

        ans->catchTheBall = good;

        return ans;
    }

    void process() {
        this->ball->updateBallInfo(5000, 2400, 0, 1000);
        //cout << this->player[3]->playerPosition.X << " " << this->player[3]->playerPosition.Y << " xxx\n";
        while (this->ball->speedCalc() - 120 > 0) {
            Player* ourAttacker = this->getOurAttacker();
            for (int i = 0; i < 5; i++) {
                if (this->player[i] == ourAttacker) {
                    cout << i << " found\n";
                }
            }

            int xxx = this->ball->possible[ourAttacker->catchTheBall].X - ourAttacker->playerPosition.X;
            int yyy = this->ball->possible[ourAttacker->catchTheBall].Y - ourAttacker->playerPosition.Y;
            double tmp = 1.0 + double(sqr(xxx)) / double(sqr(yyy));

            int newSpeedY = sqrt(double(90000 / tmp));
            int newSpeedX = sqrt(sqr(300) - sqr(newSpeedY));
            if (newSpeedX * xxx < 0) newSpeedX *= -1;
            if (newSpeedY * yyy < 0) newSpeedY *= -1;
            ourAttacker->playerPosition = vec2(ourAttacker->playerPosition.X + newSpeedX, ourAttacker->playerPosition.Y + newSpeedY);

            //cout << this->player[3]->playerPosition.X << " " << this->player[3]->playerPosition.Y << " xxx\n";

            this->ball->ballPosition = this->ball->possible[1];
            this->ball->ballSpeed = this->ball->speed[0];
            this->ball->predictBall();
        }
    }
};

int main() {

    gameMatch* myMatch = new gameMatch();

    myMatch->process();

    return 0;
}
