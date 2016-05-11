#include "motionPlan.h"

//using namespace waypointTrajectory;

template <typename T>
string NumberToString ( T Number )
{
    stringstream ss;
    ss << Number;
    return ss.str();
}

motionPlan::motionPlan()
{
    ts.update();
    listTrajectory.clear();
    haveNewMsg = false;

}
motionPlan::~motionPlan()
{
    listTrajectory.clear();
}

void motionPlan::addTrajectory(const waypointTrajectory &ctrlPtTrajectory)
{
    listTrajectory.push_back(ctrlPtTrajectory);
}

void motionPlan::clearTrajectory()
{
    listTrajectory.clear();
}

deque<waypointTrajectory>& motionPlan::getListTrajectory()
{
    return listTrajectory;
}

void motionPlan::sendPlan()
{
    Bottle &outPlan = prepare();
    outPlan.clear();
    for (int i=0; i<listTrajectory.size(); i++)
    {
        waypointTrajectory trajectory = listTrajectory[i];
        Bottle trajectPack, ctrlPt, nWp, nDim, listWaypoints, wp, coordinate;
        vector<Vector> waypoints = trajectory.getWaypoints();

        ctrlPt.addString("control-point");
        ctrlPt.addString(trajectory.getCtrlPointName());
        trajectPack.addList().read(ctrlPt);

        nWp.addString("number-waypoints");
        nWp.addInt(trajectory.getNbWaypoint());
        trajectPack.addList().read(nWp);

        nDim.addString("number-dimension");
        nDim.addInt(trajectory.getDimension());
        trajectPack.addList().read(nDim);


        for (int j=0; j<waypoints.size(); j++)
        {
            wp.clear(); coordinate.clear();
            string wpName = "waypoint_" + NumberToString(j);
            wp.addString(wpName);
            for (int k=0; k<trajectory.getDimension(); k++)
                coordinate.addDouble(waypoints[j][k]);
            wp.addList().read(coordinate);
            listWaypoints.addList().read(wp);
        }

        trajectPack.append(listWaypoints);


        outPlan.addList().read(trajectPack);
    }

    ts.update();
    setEnvelope(ts);
    write();


}

bool& motionPlan::gotNewMsg()
{
    return haveNewMsg;
}

void motionPlan::setNewMsg(const bool &value)
{
    haveNewMsg = value;
}

void motionPlan::onRead(Bottle &inPlan)
{
    clearTrajectory();
    haveNewMsg = true;


    if (!inPlan.isNull())
    {
//        Bottle* inListTrajectories = inPlan->get(0).asList();
        int numberCtrlPoints = inPlan.size();
        printf("numberCtrlPoints= %d\n",numberCtrlPoints);
//        if (inListTrajectories != NULL)
//        {

            vector<Vector> trajectory;
            int numberWaypoint, numberDimension;

            for (int i=0; i<numberCtrlPoints; i++)
            {
                printf("i= %d\n",i);
//                Bottle* inListTrajectory = inPlan->get(i).asList();
                if (Bottle* inListTrajectory = inPlan.get(i).asList())
                {
                    string ctrlPtName = inListTrajectory->find("control-point").asString();
                    printf("ctrlPtName= %s\n", ctrlPtName.c_str());
                    waypointTrajectory wpTraject;
                    wpTraject.setCtrlPointName(ctrlPtName);


                    if (int tempMsg = inListTrajectory->find("number-waypoints").asInt())
                    {
                        numberWaypoint = tempMsg;
//                        printf("numberWaypoint = %d\n",numberWaypoint);
                    }

                    if (int tempMsg = inListTrajectory->find("number-dimension").asInt())
                    {
                        numberDimension = tempMsg;
//                        printf("numberDimension = %d\n",numberDimension);
                    }

                    trajectory.clear();
                    for (int j=0; j<numberWaypoint; j++)
                    {
                        Vector waypoint(numberDimension,0.0);
                        string wpName = "waypoint_" + NumberToString(j);
                        if (Bottle* coordinate = inListTrajectory->find(wpName).asList())
                        {
                            if (coordinate->size()==numberDimension)
                            {
                                for (int k=0; k<numberDimension; k++)
                                {
                                    waypoint[k]=coordinate->get(k).asDouble();
//                                    printf("waypoint[%d]= %f\n",k,waypoint[k]);
                                }
                                trajectory.push_back(waypoint);
                            }
                        }

                    }
                    wpTraject.setWaypoints(trajectory);
//                    listTrajectory.push_back(wpTraject);
                    addTrajectory(wpTraject);

                }

            }

    }
//    printf("listTrajectory.size()= %d\n",(int) listTrajectory.size());
//    if (listTrajectory.size()>0)
//    {
//        for (int i=0; i<listTrajectory.size(); i++)
//        {
//            printf("i= %d\n",i);
//            vector<Vector> tempTrajectory = listTrajectory[i].getWaypoints();
//            for (int j=0; j<tempTrajectory.size(); j++)
//            {
//                printf("Waypoint[%d] = %f, %f, %f\n",j,tempTrajectory[j][0],tempTrajectory[j][1],tempTrajectory[j][2]);
//            }
//        }
//    }
}

void motionPlan::receivePlan()
{
    Bottle* inPlan = read(false);
//    haveNewMsg = true;
    clearTrajectory();

    if (inPlan != NULL)
    {
//        Bottle* inListTrajectories = inPlan->get(0).asList();
        int numberCtrlPoints = inPlan->size();
        printf("numberCtrlPoints= %d\n",numberCtrlPoints);
//        if (inListTrajectories != NULL)
//        {

            vector<Vector> trajectory;
            int numberWaypoint, numberDimension;

            for (int i=0; i<numberCtrlPoints; i++)
            {
                printf("i= %d\n",i);
//                Bottle* inListTrajectory = inPlan->get(i).asList();
                if (Bottle* inListTrajectory = inPlan->get(i).asList())
                {
                    string ctrlPtName = inListTrajectory->find("control-point").asString();
                    printf("ctrlPtName= %s\n", ctrlPtName.c_str());
                    waypointTrajectory wpTraject;
                    wpTraject.setCtrlPointName(ctrlPtName);


                    if (int tempMsg = inListTrajectory->find("number-waypoints").asInt())
                    {
                        numberWaypoint = tempMsg;
//                        printf("numberWaypoint = %d\n",numberWaypoint);
                    }

                    if (int tempMsg = inListTrajectory->find("number-dimension").asInt())
                    {
                        numberDimension = tempMsg;
//                        printf("numberDimension = %d\n",numberDimension);
                    }

                    trajectory.clear();
                    for (int j=0; j<numberWaypoint; j++)
                    {
                        Vector waypoint(numberDimension,0.0);
                        string wpName = "waypoint_" + NumberToString(j);
                        if (Bottle* coordinate = inListTrajectory->find(wpName).asList())
                        {
                            if (coordinate->size()==numberDimension)
                            {
                                for (int k=0; k<numberDimension; k++)
                                {
                                    waypoint[k]=coordinate->get(k).asDouble();
//                                    printf("waypoint[%d]= %f\n",k,waypoint[k]);
                                }
                                trajectory.push_back(waypoint);
                            }
                        }

                    }
                    wpTraject.setWaypoints(trajectory);
                    listTrajectory.push_back(wpTraject);

                }

            }

    }
    printf("listTrajectory.size()= %d\n",(int) listTrajectory.size());
    if (listTrajectory.size()>0)
    {
        for (int i=0; i<listTrajectory.size(); i++)
        {
            printf("i= %d\n",i);
            vector<Vector> tempTrajectory = listTrajectory[i].getWaypoints();
            for (int j=0; j<tempTrajectory.size(); j++)
            {
                printf("Waypoint[%d] = %f, %f, %f\n",j,tempTrajectory[j][0],tempTrajectory[j][1],tempTrajectory[j][2]);
            }
        }
    }

}




