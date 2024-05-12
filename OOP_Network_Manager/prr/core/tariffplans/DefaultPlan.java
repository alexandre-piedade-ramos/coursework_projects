package prr.core.tariffplans;

import prr.core.client.*;
import prr.core.client.clientlevels.*;
import prr.core.communications.*;
import java.io.Serializable;

public class DefaultPlan extends TariffPlan implements Serializable{

    private static final long serialVersionUID = 202208091753L;


    public DefaultPlan(){
        super("DefaultPlan");
    }
    //Level polymorphism woulnd't compile. Couldn't figure out why

    public double computeCommCost(String level, TextComm comm){
        int len=comm.getLength();
        if(level.equals("NORMAL")){
            if(len<50){
                    return 10;
                }
                else if(len<100){
                    return 16;
                }
                else{
                    return 2*len;
                }
        }
        if(level.equals("GOLD")){
            if(len<50){
                    return 10;
                }
                else if(len<100){
                    return 10;
                }
                else{
                    return 2*len;
                }
        }
        if(level.equals("PLATINUM")){
            if(len<50){
                    return 0;
                }
                else if(len<100){
                    return 4;
                }
                else{
                    return 4;
                }
        }
        else{
            return -1;
        }
    }
    public double computeCommCost(String level, VoiceComm comm){
        int len=comm.getLength();
        if(level.equals("NORMAL")){
            return len*20;
        }
        if(level.equals("GOLD")){
            return len*10;
        }
        if(level.equals("PLATINUM")){
           return len*10;
        }
        else{
            return -1;
        }
    }
    public double computeCommCost(String level, VideoComm comm){
        int len=comm.getLength();
        if(level.equals("NORMAL")){
            return len*30;
        }
        if(level.equals("GOLD")){
            return len*20;
        }
        if(level.equals("PLATINUM")){
           return len*10;
        }
        else{
            return -1;
        }
    }

}
