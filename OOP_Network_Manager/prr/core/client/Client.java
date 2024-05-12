package prr.core.client;


import prr.core.terminal.*;
import prr.core.communications.*;
import prr.core.client.clientlevels.*;
import prr.core.tariffplans.*;
import prr.core.notifications.*;

import java.util.Collection;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;

import prr.core.exception.*;


import java.io.Serializable;

public class Client implements Serializable{

    private static final long serialVersionUID = 202208091753L;

    private String _key;
    private String _name;
    private Integer _taxNumber;
    private ClientLevel _level;
    private double _debts;
    private double _payments;
    private TariffPlan _tariffPlan;
    private boolean _receiveNotifications;
    private List<String> _notifications;
    private Map<String,Terminal> _terminals = new HashMap<>();
    private ArrayList<Comm> _comms;
    private NotifDeliveryMethod _notifDeliveryMethod;
    private List<Notification> _notifs;


    public Client(String key, String name, Integer taxNumber){
        _key=key;
        _name=name;
        _taxNumber=taxNumber;
        _level=new NormalLevel(this);
        _receiveNotifications=true;
        _comms=new ArrayList<>();
        _tariffPlan=new DefaultPlan();
        _notifDeliveryMethod= new SendToClient(this);
        _notifs= new ArrayList<>();
    }

    public String getKey(){
        return _key;
    }

    @Override
    public String toString(){
        String notifsOn = _receiveNotifications?"YES":"NO";
        
        return "CLIENT|" + _key +"|"+ _name +"|"+ _taxNumber +"|" +
                    _level + "|" + notifsOn + "|" +
                    _terminals.size() + "|" +
                    Math.round(_payments) + "|" +
                    Math.round(_debts);
    }

    @Override
    public boolean equals(Object other){
        if(!(other instanceof Client)) return false;
        Client otherClient = (Client) other;
        return _key.toLowerCase()==otherClient.getKey().toLowerCase();
    }

    public double calcBalance(){
        return _payments-_debts;

    }
    public boolean notifsOn(){
        return _receiveNotifications;
    }
    public void addTerminal(Terminal terminal){
        _terminals.put(terminal.getId(), terminal);
    }

    public void setLevel(ClientLevel level){
        _level=level;
    }

    public ClientLevel getLevel(){
        return _level;
    }
    
    public List<Comm> getLastComms(int num){
        return _comms.subList(_comms.size()-1-num, _comms.size());
    }

    public TariffPlan getTariffPlan(){
        return _tariffPlan;
    }

    public boolean disableNotifs(){
        if(_receiveNotifications){
            _receiveNotifications=false;
            return true;
        }
        else{
            return false;
        }
    }

    public boolean enableNotifs(){
        if(!_receiveNotifications){
            _receiveNotifications=true;
            return true;
        }
        else{
            return false;
        }
    }

    public void addDebt(double val){
        _debts+=val;
    }

    public void addPayment(double val){
        _debts-=val;
        _payments+=val;
    }

    public double getDebts(){
        return _debts;
    }

    public double getPayments(){
        return _payments;
    }

    public void addNotif(Notification notif){
        if(_notifs.contains(notif)){
            return;
        }
        _notifs.add(notif);
    }

    public List<String> notifsToStrings(){
        int len=_notifs.size();
        LinkedList<String> toReturn=new LinkedList<>();
        if(len==0){
            toReturn.add("");
            return toReturn;
        }
        for(Notification n:_notifs){
            toReturn.add(n.toString());
        }
        _notifs.clear();
        return toReturn;
    }

    public NotifDeliveryMethod getDeliveryMethod(){
        return _notifDeliveryMethod;
    }

    public List<Comm> getComms(){
        return _comms;
    }
}