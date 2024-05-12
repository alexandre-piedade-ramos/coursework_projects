package prr.core.comparator;
import java.util.*;
import java.io.Serializable;
import prr.core.client.Client;

public class ClientKeyComparator implements Comparator<Client>, Serializable{
    
    private static final long serialVersionUID = 202208091753L;
    
    public int compare(Client c1, Client c2){
        return c1.getKey().toLowerCase().compareTo(c2.getKey().toLowerCase());
    }

}
