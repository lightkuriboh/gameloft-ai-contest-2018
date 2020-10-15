#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
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
const int maxDistPass = 3600;
const int minDistPass = 1000;
const int maxDistWait = 2400;
const int maxDistShoot = 4400;
const int minKheDistPass = 500;
const int oo = 1000000000;
const vec2 nullPosition = vec2(-1, -1);

const int goalWidth = 2800;

vector<int> fieldLeft = {1},
            fieldRight = {3, 2};

bool inFieldLeft (const int &state) {
    for (auto x: fieldLeft) {
        if (state == x) {
            return true;
        }
    }
    return false;
}

bool inFieldRight (const int &state) {
    for (auto x: fieldRight) {
        if (state == x) {
            return true;
        }
    }
    return false;
}

vector <vec2> kickTargets = {
                                vec2(MAP_W, 3652),
                                vec2(MAP_W, 3752),
                                vec2(MAP_W, 3852),
                                vec2(MAP_W, 3952),
                                vec2(MAP_W, 4052),
                                vec2(MAP_W, 4152),
                                vec2(MAP_W, 4252),
                                vec2(MAP_W, 4352),
                                vec2(MAP_W, 4452),
                                vec2(MAP_W, 4552),
                                vec2(MAP_W, 4652),
                                vec2(MAP_W, 4752),
                                vec2(MAP_W, 4852),
                                vec2(MAP_W, 4952),
                                vec2(MAP_W, 5052),
                                vec2(MAP_W, 5152),
                                vec2(MAP_W, 5252),
                                vec2(MAP_W, 5352),
                                vec2(MAP_W, 5452),
                                vec2(MAP_W, 5552),
                                vec2(MAP_W, 5652),
                                vec2(MAP_W, 5752),
                                vec2(MAP_W, 5852),
                                vec2(MAP_W, 5952),
                            };

const int maxMove = 300;
const int maxKick = 200;
const int deaccelation = 120;

class Ball {
public:
    vec2 ballPosition;
    vec2 ballSpeed;
    vector <vec2> possible;

    Ball (const int &x, const int &y) {
        this->ballPosition = mp(x, y);
    }

    void updateBallInfo (const int &x, const int &y, const int &sx, const int &sy) {
        this->ballPosition = mp(x, y);
        this->ballSpeed = mp(sx, sy);

        this->possible = this->predictBall(this->ballPosition, this->ballSpeed);
    }

    int speedCalc() {
        return sqrt(sqr(this->ballSpeed.X) + sqr(this->ballSpeed.Y));
    }

    vec2 calcBallSpeedXY (int speedNow, vec2 _ballSpeed) {

        if (speedNow <= 120) {
            return vec2(0, 0);
        }
        if (_ballSpeed.Y != 0) {

            double tmp = 1.0 + double(sqr(_ballSpeed.X)) / double(sqr(_ballSpeed.Y));
            int newSpeedY = sqrt(double(sqr(speedNow)) / tmp);

            int newSpeedX = sqrt(sqr(speedNow) - sqr(newSpeedY));

            if (newSpeedX * _ballSpeed.X < 0) {
                newSpeedX *= -1;
            }
            if (newSpeedY * _ballSpeed.Y < 0) {
                newSpeedY *= -1;
            }

            return vec2(newSpeedX, newSpeedY );
        } else {

            int newSpeedX = speedNow;
            if (newSpeedX * _ballSpeed.X < 0) {
                newSpeedX *= -1;
            }
            return vec2(newSpeedX, 0);

        }
    }

    vector <vec2> predictBall(const vec2& _ballPosition, const vec2 &_ballSpeed) {
        vector <vec2> ans;
        int speedNow = sqrt(sqr(_ballSpeed.X) + sqr(_ballSpeed.Y));
        ans.push_back(_ballPosition);

        vec2 curPosition = _ballPosition;
        vec2 curSpeed = _ballSpeed;

        while (speedNow >= deaccelation) {

            speedNow -= deaccelation;

            if (_ballSpeed.Y != 0) {

                double tmp = 1.0 + double(sqr(_ballSpeed.X)) / double(sqr(_ballSpeed.Y));
                int newSpeedY = sqrt(double(sqr(speedNow)) / tmp);

                if (curSpeed.Y != 0) {
                    newSpeedY *= curSpeed.Y / abs(curSpeed.Y);
                }

                int newSpeedX = sqrt(sqr(speedNow) - sqr(newSpeedY));

                if (curSpeed.X != 0) {
                     newSpeedX *= curSpeed.X / abs(curSpeed.X);
                }

                curSpeed = mp(newSpeedX, newSpeedY );
            } else {

                if (curSpeed.X != 0) {
                    curSpeed = mp(speedNow * curSpeed.X / abs(curSpeed.X), 0);
                }
                else {
                    curSpeed = mp(speedNow, 0);
                }

            }

            curPosition.X += curSpeed.X;
            curPosition.Y += curSpeed.Y;

            if (curPosition.X > MAP_W || curPosition.Y > MAP_H || curPosition.X < 0 || curPosition.Y < 0) break;

            ans.push_back(curPosition);
        }
        return ans;
    }
};

class Player {
public:
    vec2 playerPosition;
    bool isEnemy;
    bool isPaired;

    vec2 posToCatchBall;

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

    int distToBall(Ball *ball) {
        return sqrt(
            sqr(this->playerPosition.X - ball->ballPosition.X)
            + sqr(this->playerPosition.Y - ball->ballPosition.Y)
        );
    }

    void updateAction(const int &action, const int &desX, const int &desY, const int &force) {
        this->playerAction.action = action;
        this->playerAction.desX = desX;
        this->playerAction.desY = desY;
        this->playerAction.force = force;
    }

    int catchTheBall;
};

vector <vec2> initiatePosition =
                                {
                                    mp(5400, 2400)
                                    ,
                                    mp(5400, 3600)
                                    ,
                                    mp(5400, 4800)
                                    ,
                                    mp(5400, 6000)
                                    ,
                                    mp(5400, 7200)
                                };

int HALF_1 = 1;
int HALF_2 = 2;
int EXTRA = 3;
int PENALTY = 4;

class gameMatch {
public:
    Ball* ball;
    Player* player[10];
    int gameState;
    int gameTurn;
    bool weControling = false;

    gameMatch () {
        ball = new Ball(MAP_W / 2, MAP_H / 2);
        for (int i = 0; i < 5; i++) {
            player[i] = new Player(initiatePosition[i].X, initiatePosition[i].Y, false);
        }
        for (int i = 5; i < 10; i++) {
            player[i] = new Player(0, 0, true);
        }
    }

    pair<vec2, int> chosenOneAndDist (vector<vec2> balls, vector <Player*> players) {

        int minGap = oo;
        int secondsToCatch = oo;
        vec2 finalPos = nullPosition;

        for (Player* _player: players) {
            int mn = oo;
            int tmpSecond = oo;
            vec2 tmpPos;
            for (int j = 0; j < (int)balls.size(); j++) {
                vec2 pos = balls[j];
                int dist = this->Dist(pos, _player->playerPosition) - maxKick;
                int step = dist / maxMove;
                int gap = j - step;
                if (gap > 0 && j < minGap) {
                    minGap = j;
                    finalPos = pos;
                    secondsToCatch = j;
                }
            }
        }

        return mp(finalPos, secondsToCatch);
    }

    pair<vec2, int> posBallCanBeCatch(Player* attacker, vec2 _ballPosition, vec2 A, vec2 B) {

        int velo = 10 * this->forceToKick(A, B);
        vec2 vVec = vec2(B.X - A.X, B.Y - A.Y);
        vec2 ballVeloXY = this->ball->calcBallSpeedXY(velo, vVec);
        vector <vec2> posBalls = this->ball->predictBall(_ballPosition, ballVeloXY);

        vec2 ans = nullPosition;

        vector <Player*> we, they;

        for (int i = 0; i < 5; i++) {
            if (this->player[i] != attacker) {
                we.push_back(this->player[i]);
            }
        }

        for (int i = 5; i < 10; i++) {
            they.push_back(this->player[i]);
        }

        pair<vec2, int> us   = this->chosenOneAndDist(posBalls,   we);
        pair<vec2, int> them = this->chosenOneAndDist(posBalls, they);

        vec2 ourNearest = us.X;
        vec2 theirNearest = them.X;

        int ourSteps = us.Y, theirSteps = them.Y;

        if (ourSteps == oo && theirSteps == oo) {
            return mp(nullPosition, -1);
        }

        if (theirSteps <= ourSteps) {
            return mp(theirNearest, -theirSteps);
        }

        return mp(ourNearest, ourSteps);

    }

    vec2 convertTargetPostition(vec2 target) {
        if (inFieldLeft(this->gameState)) {
            return target;
        }
        if (inFieldRight(this->gameState)) {
            return vec2(MAP_W - target.X, target.Y);
        }
    }

    bool inMap(vec2 pos) {
        return pos.X >= 0 && pos.X <= MAP_W && 0 <= pos.Y && pos.Y <= MAP_H;
    }

    int Dist(vec2 a, vec2 b) {
        return sqrt(sqr(a.X - b.X) + sqr(a.Y - b.Y));
    }

    Player* enemyHoldingBall () {
        for (int i = 5; i < 10; i++) {
            if (this->player[i]->distToBall(this->ball) < maxKick) {
                return this->player[i];
            }
        }
        return nullptr;
    }

    bool weCanKickBall (Player* attacker) {

        if (attacker->distToBall(this->ball) <= maxKick) {
            return true;
        }
        return false;
    }

    void parseData(const string &s) {
        istringstream is (s);
        int n;
        vector <int> vt;
        while (is >> n) {
            vt.push_back(n);
        }

        this->gameTurn = vt[0];

        if (myTeamID == 1) {
            vector <int> tmp;
            for (auto x: vt) tmp.push_back(x);
            vt.clear();
            for (int i = 0; i < 8; i++)
                vt.push_back(tmp[i]);
            for (int i = 23; i < 38; i++)
                vt.push_back(tmp[i]);
            for (int i = 8; i < 23; i++)
                vt.push_back(tmp[i]);
        }

        this->ball->updateBallInfo(vt[4], vt[5], vt[6], vt[7]);

        for (int i = 0; i < 10; i++) {
            int id = vt[8 + 3 * i];
            if (i > 4) id += 5;
            this->player[id]->updatePlayerPosition(vt[9 + 3 * i], vt[10 + 3 * i]);
        }
        this->gameState = vt[3];
    }

    Player* getTheUselessEnemy () {
        Player* uselessEnemy;

        if (inFieldLeft(this->gameState)) {

            int mxX = -1;

            for (int i = 5; i < 10; i++) {
                if (this->player[i]->playerPosition.X > mxX) {
                    mxX = this->player[i]->playerPosition.X;
                    uselessEnemy = this->player[i];
                }
            }
        }
        else
        if (inFieldRight(this->gameState)) {
            int mnn = 1000000000;
            for (int i = 5; i < 10; i++) {
                if (this->player[i]->playerPosition.X < mnn) {
                    mnn = this->player[i]->playerPosition.X;
                    uselessEnemy = this->player[i];
                }
            }
        }
        return uselessEnemy;
    }

    Player* getOurAttacker () {

        for (int i = 0; i < 5; i++) {
            if (this->weCanKickBall(this->player[i])) {
                return this->player[i];
            }
        }

        Player* ans = nullptr;
        vec2 finalPos;

        int mnStep = 1000000000;
        int good = -1;
        int mnDistToBall = 1000000000;
        for (int i = 0; i < 5; i++) {
            int mnNow = 1000000000;
            int tmpGood = -1;
            vec2 tmpPos;
            int tmpMnDistToBall = mnNow;

            for (int j = 0; j < (int)this->ball->possible.size(); j++) {
                auto pos = this->ball->possible[j];
                int distPlayerToPos = this->Dist(this->player[i]->playerPosition, pos) - maxKick;
                int stepToBall = distPlayerToPos / maxMove;
                int gap = j - stepToBall;
                if (gap > 0) {
                    if (gap < mnNow || (gap == mnNow && tmpMnDistToBall > distPlayerToPos)) {
                        mnNow = gap;
                        tmpGood = j;
                        tmpPos = this->ball->possible[j];
                        tmpMnDistToBall = distPlayerToPos;
                    }
                }
            }

            for (int j = 0; j < (int)this->ball->possible.size() - 1; j++) {
                vec2 pos1 = this->ball->possible[j];
                vec2 pos2 = this->ball->possible[j + 1];
                vec2 vVec = vec2(pos2.X - pos1.X, pos2.Y - pos1.Y);
                vec2 newVVec = vec2(vVec.X / 10, 0);
                if (vVec.X != 0) {
                    newVVec.Y = newVVec.X * vVec.Y / vVec.X;
                } else {
                    newVVec.X = 0;
                    newVVec.Y = min(150, vVec.Y);
                }
                if (newVVec.X * vVec.X < 0) newVVec.X *= -1;
                if (newVVec.Y * vVec.Y < 0) newVVec.Y *= -1;
                for (int l = 1; l <= 10; l++) {
                    vec2 nxPos = vec2(pos1.X + l * newVVec.X, pos1.Y + l * newVVec.Y);
                    int dist = this->Dist(this->player[i]->playerPosition, nxPos) - maxKick;
                    int steps = dist / maxMove;
                    if (steps % maxMove > 150) steps++;
                    int gap = j - steps;
                    if (gap > 0) {

                        if (gap < mnNow || (gap == mnNow && dist < tmpMnDistToBall)) {
                            mnNow = gap;
                            tmpGood = j;
                            tmpPos = nxPos;
                            tmpMnDistToBall = dist;
                        }

                    }
                }
            }

            if (mnNow > 0 && (mnNow < mnStep || (mnNow == mnStep && tmpMnDistToBall < mnDistToBall))) {
                mnStep = mnNow;
                ans = this->player[i];
                good = tmpGood;
                finalPos = tmpPos;
                mnDistToBall = tmpMnDistToBall;
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

        ans->posToCatchBall = finalPos;
        ans->catchTheBall = good;

        return ans;
    }

    vec2 calcGoodPositionInDefend(Player* enemy, bool isDangerous) {

        if (isDangerous) {
            return vec2(this->higherPosition(enemy->playerPosition.X, -500), enemy->playerPosition.Y);
        }

        vec2 originalPoint = this->ball->ballPosition;

        if (isDangerous) {
            originalPoint = this->convertTargetPostition(vec2(0, MAP_H / 2));
        }

        vec2 Vector = vec2(originalPoint.X - enemy->playerPosition.X, originalPoint.Y - enemy->playerPosition.Y);

        if (Vector == vec2(0, 0)) {
            return enemy->playerPosition;
        }

        if (Vector.Y != 0) {

            int yyy = 3 * maxKick;

            int xxx = yyy * Vector.X / Vector.Y;

            if (max(xxx, yyy) > 3 * maxKick) {
                int Ratio = abs(max(xxx, yyy)) / (3 * maxKick);
                xxx /= Ratio;
                yyy /= Ratio;
            }

            if (Vector.X * xxx < 0) xxx *= -1;
            if (Vector.Y * yyy < 0) yyy *= -1;
            return vec2(enemy->playerPosition.X + xxx, enemy->playerPosition.Y + yyy);
        } else {
            int xxx = 3 * maxKick;
            xxx = abs(xxx);
            while (xxx > 3 * maxKick) {
				if (xxx > 1000) xxx -= 800;
                xxx -= 10;
            }
            if (Vector.X * xxx < 0) xxx *= -1;
            return vec2(enemy->playerPosition.X + xxx, enemy->playerPosition.Y);
        }
    }

    vec2 theBestKickPoint(Player* attacker) {
        int mxx = -1;
        vec2 ans;
        int longest = maxDistShoot;
        for (auto targett: kickTargets) {
            auto target = this->convertTargetPostition(targett);
            int mnn = 1000000000;
            for (int i = 5; i < 10; i++) {
                mnn = min(mnn, this->Dist(this->player[i]->playerPosition, target));
            }
            if (mnn > mxx && this->Dist(attacker->playerPosition, target) <= longest) {
                mxx = mnn;
                ans = target;
            }
        }
        return ans;
    }

    bool weCanShootFinish(Player* attacker) {
        for (auto pos: kickTargets) {
            if (this->Dist(this->ball->ballPosition, this->convertTargetPostition(pos)) <= maxDistShoot) {
                return true;
            }
        }
        return false;
    }


    Player* dangerousEnemy(Player* otherDanger) {
        Player* ans = nullptr;
        if (inFieldLeft(this->gameState)) {
            int mn = 1000000000;
            for (int i = 5; i < 10; i++) if (this->player[i] != otherDanger) {
                if (this->player[i]->playerPosition.X < mn) {
                    mn = this->player[i]->playerPosition.X;
                    ans = this->player[i];
                }
            }
        } else
        if (inFieldRight(this->gameState)) {
            int mx = -1;
            for (int i = 5; i < 10; i++) if (this->player[i] != otherDanger) {
                if (this->player[i]->playerPosition.X > mx) {
                    mx = this->player[i]->playerPosition.X;
                    ans = this->player[i];
                }
            }
        }
        return ans;
    }

    bool isHigherDependOnState(Player* x, Player* xx) {
        if (inFieldLeft(this->gameState)) {
            return x->playerPosition.X > xx->playerPosition.X;
        }
        return x->playerPosition.X < xx->playerPosition.X;
    }


    int forceToKick(vec2 A, vec2 B) {
        int dist = this->Dist(A, B);
        double hs = 3.5;
        if (dist <= 1200) {
            hs = 2.7;
        }
        int velo = sqrt(hs * dist * deaccelation);
        return velo / 10;
    }

    int higherPosition(const int &x, const int &_delta) {
        int delta = _delta;

        if (inFieldRight(this->gameState)) {
            delta = -delta;
        }

        return x + delta;
    }

    int distToOtherEnemies (Player* cur) {
        int mn = oo;
        for (int i = 5; i < 10; i++) {
            mn = min(mn, this->Dist(this->player[i]->playerPosition, cur->playerPosition));
        }
        return mn;
    }

    int dx[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1},
        dy[9] = {-1, 0, 1, -1, 1, 0, -1, 0, 1};


    vec2 someGoodPositionOneToPass (Player* attacker, bool needHigher) {

        vec2 ans = vec2(-9999, -9999);
        Player* receiver = nullptr;

        int maxRange = 5;
        int mx = -1;

        int mn = oo;

        for (int i = 0; i < 5; i++) {
            if (this->player[i] != attacker) {

                if (!needHigher || this->isHigherDependOnState(this->player[i], attacker)) {

                    if (this->Dist(attacker->playerPosition, this->player[i]->playerPosition) > maxDistPass) {
                        continue;
                    }

                    vec2 curPos = this->player[i]->playerPosition;

                    for (int d = 0; d < 9; d++) {
                        for (int range = 1; range <= maxRange; range++) {

                            vec2 nxPos = vec2(curPos.X + dx[d] * range * maxKick, curPos.Y + dy[d] * range * maxKick);
                            pair<vec2, int> ngon = this->posBallCanBeCatch(attacker, this->ball->ballPosition, this->ball->ballPosition, nxPos);
                            if (ngon.Y >= 0 && this->distToOtherEnemies(this->player[i]) > mx) {
                                receiver = this->player[i];
                                ans = ngon.X;
                                mn = ngon.Y;
                                mx = this->distToOtherEnemies(this->player[i]);
                            }
                        }
                    }

                }

            }
        }

        if (receiver != nullptr) {
            receiver->updateAction(MOVE, ans.X, ans.Y, 0);
        }

        return ans;
    }

    void makeViewAllTeam(Player* attacker) {
        int mxRange = 5;
        for (int i = 0; i < 5; i++) {
            if (this->player[i] != attacker) {

                vec2 curPos = this->player[i]->playerPosition;
                vec2 tarPos = vec2(this->higherPosition(curPos.X, 300), curPos.Y);

                int mx = -1;
                int st = 6, fi = 9;
                //int st = 7, fi = 7;

                if (inFieldRight(this->gameState)) {
                    st = 0; fi = 3;
                    //st = 1; fi = 1;
                }

                for (int d = st; d < fi; d++) {
                    for (int range = 1; range <= mxRange; range++) {
                        vec2 nxPos = vec2(curPos.X + dx[d] * range * maxKick, curPos.Y + dy[d] * range * maxKick);

                        int speedNow = 10 * this->forceToKick(this->ball->ballPosition, nxPos);
                        vec2 vVec = vec2(nxPos.X - this->ball->ballPosition.X, nxPos.Y - this->ball->ballPosition.Y);

                        vec2 vecBallSpeed = this->ball->calcBallSpeedXY(speedNow, vVec);
                        vector <vec2> predictBall = this->ball->predictBall(this->ball->ballPosition, vecBallSpeed);

                        int mn = 1000000000;
                        for (int j = 1; j < (int)predictBall.size(); j++) {
                            for (int t = 5; t < 10; t++) {
                                mn = min(mn, this->Dist(predictBall[j], this->player[t]->playerPosition));
                            }
                        }

                        if (mn != 1000000000 && mn >= minKheDistPass && mn > mx) {
                            mx = mn;
                            tarPos = nxPos;
                        }
                    }
                }
                if (mx != -1) {
                    this->player[i]->updateAction(MOVE, tarPos.X, tarPos.Y, 0);
                }
            }
        }
    }

    void reArrangeTeam(Player* attacker) {
        vector <Player*> positions;
        for (int i = 0; i < 5; i++) {
            positions.push_back(this->player[i]);
        }
        sort(positions.begin(), positions.end(), [&](Player* A, Player* B){
             return A->playerPosition.X < B->playerPosition.X;
        });

        int center = 0;
        for (int i = 0; i < 5; i++) {
            if (positions[i] == attacker) {
                center = i;
            }
        }

        for (int i = center - 1; i > -1; i--) {
            if (this->Dist(positions[i]->playerPosition, positions[i + 1]->playerPosition) > maxDistWait) {
                vec2 tarPos = positions[i + 1]->playerPosition;
                positions[i]->updateAction(MOVE, tarPos.X, tarPos.Y, 0);
            }
        }
        for (int i = center + 1; i < 5; i++) {
            if (this->Dist(positions[i]->playerPosition, positions[i - 1]->playerPosition) > maxDistWait) {
                vec2 tarPos = positions[i - 1]->playerPosition;
                positions[i]->updateAction(MOVE, tarPos.X, tarPos.Y, 0);
            }
        }
    }

    bool noHigherEnemy(Player* attacker) {
        for (int i = 5; i < 10; i++) {
            if (this->isHigherDependOnState(this->player[i], attacker) && this->Dist(this->player[i]->playerPosition, attacker->playerPosition) < 2100) {
                return false;
            }
        }
        return true;
    }

    bool makeChoiceOfPlayerToPass (Player* attacker, bool needHigher) {

        int mxDist = -1;
        Player* targetTeamate = nullptr;

        for (int i = 0; i < 5; i++) {
            if (this->player[i] != attacker) {
                int distNow = this->Dist(this->player[i]->playerPosition, attacker->playerPosition);
                if (distNow <= maxDistPass) {
                    int mnDist = 1000000000;
                    for (int j = 5; j < 10; j++) {
                        mnDist = min(mnDist, this->Dist(this->player[i]->playerPosition, this->player[j]->playerPosition));
                    }
                    if (mnDist > mxDist)  {

                        if (needHigher) {
                            if (this->isHigherDependOnState(this->player[i], attacker)) {
                                mxDist = mnDist;
                                targetTeamate = this->player[i];
                            }
                        } else {
                            mxDist = mnDist;
                            targetTeamate = this->player[i];
                        }
                    }
                }
            }
        }

        if (targetTeamate != nullptr) {

            vec2 tarPos = targetTeamate->playerPosition;
            int force = this->forceToKick(this->ball->ballPosition, tarPos);
            force = min(100, force);

            attacker->updateAction(SHOOT,
                                   tarPos.X,
                                   tarPos.Y,
                                    force);
            return true;

        }

        return false;
    }

    void makeRandomPass(Player* attacker) {

        if (this->makeChoiceOfPlayerToPass(attacker, true)) {
            return;
        }
        if (this->makeChoiceOfPlayerToPass(attacker, false)) {
            return;
        }

        attacker->updateAction(SHOOT, this->higherPosition(attacker->playerPosition.X, 10000), attacker->playerPosition.Y, 100);
    }

    void separateNearAllies () {
        int dangerousDist = 650;
        for (int i = 0; i < 5; i++) {
            for (int j = i + 1; j < 5; j++) {
                if (this->Dist(this->player[i]->playerPosition, this->player[j]->playerPosition) < 2000) {
                    vec2 curPosi = this->player[i]->playerPosition;
                    vec2 curPosj = this->player[j]->playerPosition;

                    if (this->player[i]->playerPosition.Y > this->player[j]->playerPosition.Y) {

                        this->player[i]->updateAction(MOVE, this->higherPosition(curPosi.X, 500), curPosi.Y + 500, 0);
                        this->player[j]->updateAction(MOVE, this->higherPosition(curPosj.X, 500), curPosj.Y - 500, 0);

                    } else {
                        this->player[i]->updateAction(MOVE, this->higherPosition(curPosi.X, 500), curPosi.Y - 500, 0);
                        this->player[j]->updateAction(MOVE, this->higherPosition(curPosj.X, 500), curPosj.Y + 500, 0);
                    }

                    if (MAP_H - curPosi.Y < dangerousDist) {

                        this->player[i]->updateAction(MOVE, this->higherPosition(curPosi.X, 300), curPosi.Y, 0);
                    }

                    if (MAP_H - curPosj.Y < dangerousDist) {

                        this->player[j]->updateAction(MOVE, this->higherPosition(curPosj.X, 300), curPosj.Y, 0);
                    }

                    if (curPosi.Y < dangerousDist) {

                        this->player[i]->updateAction(MOVE, this->higherPosition(curPosi.X, 300), curPosi.Y, 0);
                    }

                    if (curPosj.Y < dangerousDist) {

                        this->player[j]->updateAction(MOVE, this->higherPosition(curPosj.X, 300), curPosj.Y, 0);
                    }

                }
            }
        }
    }

    void attackPlan(Player* attacker) {
        int safeDist = 1801;
        int dangerousDist = 700;

        for (int i = 0; i < 5; i++) {
            vec2 curPos = this->player[i]->playerPosition;
            this->player[i]->updateAction(MOVE, this->higherPosition(curPos.X, 300), curPos.Y, 0);
        }

        this->makeViewAllTeam(attacker);

        this->separateNearAllies();

        this->reArrangeTeam(attacker);

        if (this->weCanKickBall(attacker)) {

            vector <vec2> dangerEnemies;
            Player* mostDangerousEnemy;
            int mnDist = 1000000000;

            for (int i = 5; i < 10; i++) {
                int ddd = this->Dist(attacker->playerPosition, this->player[i]->playerPosition);
                if (ddd < safeDist) {
                    dangerEnemies.push_back(this->player[i]->playerPosition);
                }
                if (ddd < mnDist) {
                    mnDist = ddd;
                    mostDangerousEnemy = this->player[i];
                }
            }

            if (this->noHigherEnemy(attacker)) {
                vec2 curPos = this->ball->ballPosition;
                curPos.X = this->higherPosition(curPos.X, 600);
                if (curPos.Y > 7200) {
                    curPos.Y -= 600;
                }
                if (curPos.Y < 2400) {
                    curPos.Y += 600;
                }

                attacker->updateAction(SHOOT,
                                       curPos.X,
                                       curPos.Y,
                                       38);
            }
            else {
                vec2 posToPassHigher = this->someGoodPositionOneToPass(attacker, true);


                if (posToPassHigher != vec2(-9999, -9999) && mnDist < safeDist - 100) {

                    int force = min(100, this->forceToKick(this->ball->ballPosition, posToPassHigher));
                    attacker->updateAction(SHOOT, posToPassHigher.X, posToPassHigher.Y, force);

                }

                //if (false) {}
                else {
                    if (dangerEnemies.size() > 1 || mnDist <= dangerousDist) {
                        vec2 posToPassHigher = this->someGoodPositionOneToPass(attacker, true);


                        if (posToPassHigher != vec2(-9999, -9999)) {
                        //if (false) {

                            int force = min(100, this->forceToKick(this->ball->ballPosition, posToPassHigher));
                            attacker->updateAction(SHOOT, posToPassHigher.X, posToPassHigher.Y, force);

                        } else {

                            vec2 posToPassNotHigher = this->someGoodPositionOneToPass(attacker, false);

                            if (posToPassNotHigher != vec2(-9999, -9999)) {

                                //cerr << "alternative";
                                int force = min(100, this->forceToKick(this->ball->ballPosition, posToPassNotHigher));
                                attacker->updateAction(SHOOT, posToPassNotHigher.X, posToPassNotHigher.Y, force);

                            } else {
                                cerr << "cannot pass";
                                this->makeRandomPass(attacker);
                            }
                        }
                    } else
                    if (mnDist > safeDist) {

                        attacker->updateAction(SHOOT,
                                               this->higherPosition(this->ball->ballPosition.X, 300),
                                               this->ball->ballPosition.Y,
                                               26);

                    } else {

                        int xxx = this->higherPosition(0, 120), yyy = 0;

                        if (dangerEnemies.size() > 0) {
                            xxx = dangerEnemies[0].X - attacker->playerPosition.X;
                            yyy = dangerEnemies[0].Y - attacker->playerPosition.Y;
                        }
                        if (dangerEnemies.size() > 1) {
                            xxx += dangerEnemies[1].X - attacker->playerPosition.X;
                            yyy += dangerEnemies[1].Y - attacker->playerPosition.Y;
                        }
                        xxx = -xxx;
                        yyy = -yyy;
                        int x, y = 60;
                        if (yyy != 0) {
                            x = y * xxx / yyy;
                        } else {
                            y = 0;
                            x = min(60, xxx);
                        }
                        if (x * xxx < 0) x = -x;
                        if (y * yyy < 0) y = -y;

                        vec2 tarPoint = vec2(this->ball->ballPosition.X + x, this->ball->ballPosition.Y + y);
                        vec2 curPoint = this->ball->ballPosition;

                        attacker->updateAction(SHOOT, tarPoint.X, tarPoint.Y, 18);
                    }
                }
            }
        }

    }

    void processNotPenalty () {
        for (int i = 0; i < 5; i++) {
            this->player[i]->catchTheBall = -1;
        }

        Player* uselessEnemy = this->getTheUselessEnemy();
        Player* ourAttacker = this->getOurAttacker();
        Player* dangerousAttacker = this->dangerousEnemy(nullptr);
        Player* dangerousAttacker2 = this->dangerousEnemy(dangerousAttacker);

        vector <int> we, they;
        for (int i = 0; i < 5; i++) {
            if (this->player[i] != ourAttacker) {
                we.push_back(i);
            }
        }
        for (int i = 5; i < 10; i++) {
            if (this->player[i] != uselessEnemy) {
                they.push_back(i);
            }
        }

        vector <int> ans;
        int mn = 1000000000;
        while (next_permutation(they.begin(), they.end())) {
            int mx = -1;
            for (int i = 0; i < 4; i++) {
                mx = max(mx, this->Dist(this->player[they[i]]->playerPosition, this->player[we[i]]->playerPosition));
            }
            if (mx < mn) {
                mx = mn;
                ans = they;
            }
        }

        they = ans;

        int we1 = -1, they1 = -1, we2 = -1, they2 = -1;

        for (int i = 0; i < 4; i++) {
            bool isDangerous = this->player[they[i]] == dangerousAttacker || this->player[they[i]] == dangerousAttacker2;
            //bool isDangerous = this->player[they[i]] == dangerousAttacker;
            if (isDangerous) {
                if (we1 == -1) {
                    we1 = we[i];
                    they1 = they[i];
                } else {
                    we2 = we[i];
                    they2 = they[i];
                }
            }
            vec2 tarPos = this->calcGoodPositionInDefend(this->player[they[i]], false);
            this->player[we[i]]->updateAction(MOVE, tarPos.X, tarPos.Y, 0);
        }

        if (this->enemyHoldingBall()) {
            this->weControling = false;
        }

        if (this->weCanKickBall(ourAttacker)) {
            this->weControling = true;
        }

        if (this->weControling) {
            this->attackPlan(ourAttacker);
        }

        if (we1 > -1 && they1 > -1 && this->player[we1] != ourAttacker) {
            vec2 tarrPos = this->calcGoodPositionInDefend(this->player[they1], true);
            this->player[we1]->updateAction(MOVE, tarrPos.X, tarrPos.Y, 0);
        }

        if (we2 > -1 && they2 > -1 && this->player[we2] != ourAttacker && abs(this->player[they1]->playerPosition.X - this->player[they2]->playerPosition.X) < 1200) {
            vec2 tarrPos = this->calcGoodPositionInDefend(this->player[they2], true);
            this->player[we2]->updateAction(MOVE, tarrPos.X, tarrPos.Y, 0);
        }

        if (this->weCanKickBall(ourAttacker)) {

            if (this->weCanShootFinish(ourAttacker)) {

                vec2 tar = this->theBestKickPoint(ourAttacker);
                ourAttacker->updateAction(SHOOT, tar.X, tar.Y, 100);

            }

        } else {

            int sz = this->ball->possible.size();

            if (!sz) {
                ourAttacker->updateAction(MOVE, this->ball->ballPosition.X, this->ball->ballPosition.Y, 0);
            } else {
                if (ourAttacker->catchTheBall != -1) {
                    if (ourAttacker->catchTheBall == -2) {
                        ourAttacker->updateAction(WAIT, ourAttacker->playerPosition.X, ourAttacker->playerPosition.Y, 0);
                        cerr << "waiting!";
                    } else {
                        //vec2 posNgon = ourAttacker->goodPosToCatchBall(ourAttacker->posToCatchBall);
                        vec2 posNgon = ourAttacker->posToCatchBall;
                        ourAttacker->updateAction(MOVE,
                                                    posNgon.X,
                                                    posNgon.Y,
                                                  0);
                    }
                } else {
                    ourAttacker->updateAction(MOVE, this->ball->possible[sz >> 1].X,
                                                    this->ball->possible[sz >> 1].Y, 0);
                    cerr << "cannot catch!";
                }
            }

        }

    }

    vec2 tarPointToCatchByGK = vec2(-1, -1);

    void processPenalty () {
        if( ( myTeamID == 0 && this->gameTurn == 0 ) || ( myTeamID == 1 && this->gameTurn == 1 ) )
        {
            tarPointToCatchByGK = vec2(-1, -1);

            std::default_random_engine generator1;
            std::uniform_int_distribution<int> distribution1(0,1);
            int ch1 = distribution1(generator1);
            std::default_random_engine generator2;
            std::uniform_int_distribution<int> distribution2(5,6);
            int ch2 = distribution2(generator2);
            std::default_random_engine generator3;
            std::uniform_int_distribution<int> distribution3(0,1);
            int ch3 = distribution3(generator3);

            vec2 target;

            if (ch3 == 0) {
                target = kickTargets[ch1];
            } else {
                target = kickTargets[ch2];
            }
            target.X = MAP_W - target.X;
            this->player[0]->updateAction(SHOOT, target.X, target.Y, 100);
        }
        else
        {
            Player* ourAttacker = this->player[0];

            if (this->weCanKickBall(ourAttacker)) {

                ourAttacker->updateAction(SHOOT, MAP_W, MAP_H, 100);

            } else {
                int sz = this->ball->possible.size();
                if (sz < 2) {
                    ourAttacker->updateAction(SHOOT, MAP_W, MAP_H, 0);
                } else {
                    if (tarPointToCatchByGK == vec2(-1, -1)) {

                        vec2 p1 = this->ball->possible[0],
                             p2 = this->ball->possible[1];

                        p1.Y = MAP_H - p1.Y;
                        p2.Y = MAP_H - p2.Y;
                        vec2 n = vec2(p1.Y - p2.Y, p2.X - p1.X);
                        int YYY = (p1.X * n.X + p1.Y * n.Y) / n.Y;
                        int dist = abs(YYY - ourAttacker->playerPosition.Y);
                        int step = dist / maxMove;
                        int trail = dist - step * maxMove;

                        if (trail <= maxKick) {
                            if (YYY > ourAttacker->playerPosition.Y) {
                                tarPointToCatchByGK = vec2(0, ourAttacker->playerPosition.Y + step * maxMove);
                            } else {
                                tarPointToCatchByGK = vec2(0, ourAttacker->playerPosition.Y - step * maxMove);
                            }
                        } else {
                            if (YYY > ourAttacker->playerPosition.Y) {
                                tarPointToCatchByGK = vec2(0, ourAttacker->playerPosition.Y + (step + 1) * maxMove);
                            } else {
                                tarPointToCatchByGK = vec2(0, ourAttacker->playerPosition.Y - (step + 1) * maxMove);
                            }
                        }
                    }
                    ourAttacker->updateAction(MOVE, tarPointToCatchByGK.X, tarPointToCatchByGK.Y, 0);
                }
            }

        }
    }

    void process() {
        if (this->gameState == PENALTY) {
            this->processPenalty();
        } else {
            this->processNotPenalty();
        }
    }

    string responseToServer () {
        string ans = "";
        for (int i = 0; i < 5; i++) {
            printf("%d %d %d %d ",
                this->player[i]->playerAction.action,


                this->player[i]->playerAction.desX,


                this->player[i]->playerAction.desY,

                this->player[i]->playerAction.force
            );
        }
        return ans;
    }

};

gameMatch* myMatch;




int main() {

    myMatch = new gameMatch;
    for (int i = 0; i < 5; i++) {
        printf("%d %d ", myMatch->player[i]->playerPosition.X, myMatch->player[i]->playerPosition.Y);
    }

    //cin >> myTeamID >> MAP_W >> MAP_H >> numTurn;
    //string s;getline(cin, s);istringstream is (s);int n;vector <int> vt;while (is >> n){vt.push_back(n);}myTeamID = vt[0];MAP_W = vt[1];MAP_H = vt[2];numTurn = vt[3];

    if (myTeamID == 1){

        swap(fieldLeft[0], fieldRight[0]);
        fieldLeft.push_back(fieldRight[1]);
        fieldRight.pop_back();

    }


    int gameTurn = 0;
    while (gameTurn++ < numTurn) {

        string input;
        getline(cin, input);
        myMatch->parseData(input);
        myMatch->process();
        myMatch->responseToServer();

    }
    return 0;
}
