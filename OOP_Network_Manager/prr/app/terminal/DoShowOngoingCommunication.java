package prr.app.terminal;

import prr.core.Network;
import prr.core.terminal.*;
import prr.core.exception.NoOngoingCommunicationException;
import pt.tecnico.uilib.menus.CommandException;

/**
 * Command for showing the ongoing communication.
 */
class DoShowOngoingCommunication extends TerminalCommand {

  DoShowOngoingCommunication(Network context, Terminal terminal) {
    super(Label.SHOW_ONGOING_COMMUNICATION, context, terminal);
  }
  
  @Override
  protected final void execute() throws CommandException {
    String commString="";
    try{
      commString = _receiver.showOngoingComm();
    }
    catch(NoOngoingCommunicationException exc){
      _display.popup(Message.noOngoingCommunication());;
    }

    _display.popup(commString);
  }
}
