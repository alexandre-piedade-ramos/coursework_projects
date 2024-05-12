package prr.core.notifications;
public interface TerminalStateObserver{
    void update();
    boolean stillObserving();
    void stopObserving();
}