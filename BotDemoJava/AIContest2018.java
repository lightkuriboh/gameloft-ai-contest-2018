/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


import java.util.Scanner;
import java.util.Random;

/**
 *
 * Don't change to another name
 */
public class AIContest2018 {


    public static final int N_PLAYER = 5;
    public static final int TEAM_ID_A = 0;
    public static final int TEAM_ID_B = 1;
    public static final int RAG_TO_SHOOT = 200;
    public static final int GOALWIDTH = 3000;
    public static final int RAG_TO_GOAL = 6000;
	public static final int PENALTY_POINT = 2400;
    public static final int HALF_1 = 1;
    public static final int HALF_2 = 2;
    public static final int MATCH_EXTRA = 3;
	public static final int MATCH_PENALTY = 4;
    /**
     * @param args the command line arguments
     */
   
    public static void main(String[] args) {
        // TODO code application logic here
        int gameTurn = 0;
		int scoreTeamA = 0;
		int scoreTeamB = 0;
		int stateMath = 0;
		int myTeamID = 1;
		int maxTurn = 0;
		int mapWidth = 14400;
		int mapHeight = 9600;
		Object[] myTeam;
		Object[] Player_A = new Object[N_PLAYER];
		Object[] Player_B = new Object[N_PLAYER];
		Object oBall = new Object();
        Position oppGoal;
		String init_team_pos = mapWidth/6 + " " + mapHeight/4 + " " + mapWidth/6 + " " + mapHeight/2 + " " + mapWidth/6 + " " + mapHeight/4*3 + " " + mapWidth/5 + " " + mapHeight/3 + " "+ mapWidth/6 + " " + mapHeight/3*2;
        System.out.print(init_team_pos);
        Scanner scTeam = new Scanner(System.in);
        //String lineTeam = scTeam .nextLine();
        myTeamID = scTeam.nextInt();
        mapWidth = scTeam.nextInt();
        mapHeight = scTeam.nextInt();
        maxTurn = scTeam.nextInt();
        //std::cin >> myTeamID >> mapWidth >> mapHeight >> maxTurn;
		//System.out.println("Maxturn:"+maxTurn);
        while (gameTurn++ < maxTurn)
        {
            Scanner scMatch = new Scanner(System.in);
            gameTurn = scMatch.nextInt();
            scoreTeamA = scMatch.nextInt();
            scoreTeamB = scMatch.nextInt();
            stateMath = scMatch.nextInt();
			// Position posBall = Position(scMatch.nextInt() , scMatch.nextInt());
            oBall.setM_pos(new Position(scMatch.nextInt() , scMatch.nextInt()));
            oBall.setM_moveSpeed(new Position(scMatch.nextInt() , scMatch.nextInt()));
            //input team players A & B			
			for (int i = 0; i < N_PLAYER; i++){
				if (Player_A[i] == null)
				{
					Player_A[i] = new Object(scMatch.nextInt(), new Position(scMatch.nextInt() , scMatch.nextInt()));
				}
				else
				{
					Player_A[i].setID(scMatch.nextInt());
					Player_A[i].setM_pos(new Position(scMatch.nextInt() , scMatch.nextInt()));
				}
			}
			for (int i = 0; i < N_PLAYER; i++){
				if (Player_B[i] == null)
				{
					Player_B[i] = new Object(scMatch.nextInt(), new Position(scMatch.nextInt() , scMatch.nextInt()));
				}
				else
				{
					Player_B[i].setID(scMatch.nextInt());
					Player_B[i].setM_pos(new Position(scMatch.nextInt() , scMatch.nextInt()));
				}
			}
			
            myTeam = Player_A;
            if (myTeamID == TEAM_ID_B){
                    myTeam = Player_B;
            }
			String outText = "";
            //Demo send players' action to server
            for (int i = 0; i < N_PLAYER; i++)
            {
				if (i == 0)
				{
					outText = "";
				}
				if (stateMath != MATCH_PENALTY)
				{
					myTeam[i].setM_action(Action.RUN);
					if (gameTurn % 20 == 0)
					{
							//Move player 0 to Ball
							if (i == 0)
							{
									myTeam[i].setM_targetPos(oBall.getM_pos());
							}
							else //random moving
							{
									Position m_targetPos_temp = new Position().Rand(mapWidth, mapHeight);
									if (myTeamID == TEAM_ID_A)
									{
											myTeam[i].setM_targetPos(m_targetPos_temp);
									}
									else
									{
											myTeam[i].setM_targetPos(new Position(mapWidth - m_targetPos_temp.GetX(), mapHeight - m_targetPos_temp.GetY()));
									}
							}				

					}
					myTeam[i].setM_force(0); //this doesn't effect to RUN action
				
					if (CanShoot(myTeam[i].getM_pos(), oBall.getM_pos()))
					{
						myTeam[i].setM_action(Action.SHOOT);

						//Detect the rival goal 
						Position goalRivalPos = new Position(mapWidth,0);
						if ((myTeamID == TEAM_ID_B && stateMath != HALF_2) || (myTeamID == TEAM_ID_A && stateMath == HALF_2))
						{
							goalRivalPos = new Position(0, 0);
						}
						if (DistancePos(goalRivalPos, oBall.getM_pos()) < RAG_TO_GOAL)
						{
								//Shoot to goal if ball nearly goal
								myTeam[i].setM_targetPos(goalRivalPos);
								myTeam[i].setM_force(100);
						}
						else
						{
								//pass ball to next player
								myTeam[i].setM_targetPos((i + 1) < N_PLAYER ? myTeam[i + 1].getM_pos() : myTeam[0].getM_pos());
								myTeam[i].setM_force(70);
						}

					}
				}
				else
				{
					if (i ==0 )
					{
						//shoot
						if ((myTeamID == TEAM_ID_A && gameTurn == 0) || (myTeamID == TEAM_ID_B && gameTurn == 1))
						{
							myTeam[i].setM_action(Action.SHOOT);
							myTeam[i].setM_force(100);
							Position goalRivalPos = new Position(0, 6000);
							myTeam[i].setM_targetPos(goalRivalPos);
						}
						//gk
						else
						{
							if (CanShoot(myTeam[i].getM_pos(), oBall.getM_pos()))
							{
								myTeam[i].setM_action(Action.SHOOT);
								myTeam[i].setM_force(100);
								Position goalRivalPos = new Position(mapWidth, mapHeight);
								myTeam[i].setM_targetPos(goalRivalPos);
							} 
							else
							{
								myTeam[i].setM_action(Action.RUN);
								myTeam[i].setM_targetPos(new Position(1000, mapHeight / 2));
							}
						}
					}
					else
					{
						myTeam[i].setM_action(Action.RUN);
						myTeam[i].setM_targetPos(new Position(0, 0));
					}
				}

				int total = myTeam[i].getM_action();
				total+=myTeam[i].getM_targetPos().GetX();
				total+=myTeam[i].getM_targetPos().GetY();
				total+=myTeam[i].getM_force();
                //System.out.print(myTeam[i].getM_action() + " " + myTeam[i].getM_targetPos().GetX() + " " + myTeam[i].getM_targetPos().GetY() + " " + myTeam[i].getM_force() + " ");
				outText = outText + myTeam[i].getM_action() + " " + myTeam[i].getM_targetPos().GetX() + " " + myTeam[i].getM_targetPos().GetY() + " " + myTeam[i].getM_force() + " ";
				if (i==N_PLAYER - 1)
				{
					System.out.print(outText);
				}
				
			}
        }
    }
    
    public static boolean CanShoot(Position mPos, Position ballPos)
    {
            if (RAG_TO_SHOOT >= DistancePos(mPos, ballPos))
                    return true;
            return false;
    }

    public static double DistancePos(Position PosA, Position PosB){
        double dis = Math.sqrt((PosA.GetX() - PosB.GetX())*(PosA.GetX() - PosB.GetX()) + (PosA.GetY() - PosB.GetY())*(PosA.GetY() - PosB.GetY()));
        return dis;
    }
}


class Position
{
    private int m_x;
    private int m_y;
    public Position()
    {
        m_x = 0;
        m_y = 0;
    }
    public Position(int x, int y)
    {
        m_x = x;
        m_y = y;
    }
    
    public void SetX(int x)
    {
        m_x = x;
    }
    public void SetY(int y)
    {
        m_y = y;
    }
    
    public int GetX()
    {
        return m_x;
    }
    
    public int GetY()
    {
        return m_y;
    }
    
    Position Rand(int mW, int mH)
    {
		Random rand = new Random();
        return new Position((int)(rand.nextInt(mW)), (int)(rand.nextInt(mH)));
    }
};

class Object
{
    private int ID;
    private Position m_pos;
    private Position m_targetPos; //direction of moving/target pos
    private Position m_moveSpeed;

    private int m_action;
    private int m_force;
    public Object()
    {
        
    }
    public Object(int ID, Position m_pos, Position m_targetPos, Position m_moveSpeed, int m_action, int m_force) {
        this.ID = ID;
        this.m_pos = m_pos;
        this.m_targetPos = m_targetPos;
        this.m_moveSpeed = m_moveSpeed;
        this.m_action = m_action;
        this.m_force = m_force;
    }
    
     public Object(int ID, Position m_pos) {
        this.ID = ID;
        this.m_pos = m_pos;
    }
    
    public int getID() {
        return ID;
    }

    public void setID(int ID) {
        this.ID = ID;
    }

    public Position getM_pos() {
        return m_pos;
    }

    public void setM_pos(Position m_pos) {
        this.m_pos = m_pos;
    }

    public Position getM_targetPos() {
        return m_targetPos;
    }

    public void setM_targetPos(Position m_targetPos) {
        this.m_targetPos = m_targetPos;
    }

    public Position getM_moveSpeed() {
        return m_moveSpeed;
    }

    public void setM_moveSpeed(Position m_moveSpeed) {
        this.m_moveSpeed = m_moveSpeed;
    }

    public int getM_action() {
        return m_action;
    }

    public void setM_action(int m_action) {
        this.m_action = m_action;
    }

    public int getM_force() {
        return m_force;
    }

    public void setM_force(int m_force) {
        this.m_force = m_force;
    }
    
};

class Action
{
    public static final int WAIT = 0;
    public static final int RUN = WAIT + 1;
    public static final int SHOOT = RUN + 1;
}