package prr.core.comparator;
import java.util.*;
import java.io.Serializable;
import prr.core.terminal.Terminal;

public class TerminalIdComparator implements Comparator<Terminal>, Serializable{
    
    private static final long serialVersionUID = 202208091753L;
    
    public int compare(Terminal c1, Terminal c2){
        return c1.getId().compareTo(c2.getId());
    }
}