package prr.core.notifications;


public interface StateObserverSubject{
    void addStateObserver(TerminalStateObserver c);
    void rmvStateObserver(TerminalStateObserver c);
    void updateStateObservers();
}