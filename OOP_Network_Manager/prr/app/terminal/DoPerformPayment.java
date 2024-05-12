package prr.app.terminal;

import prr.core.Network;
import prr.core.terminal.*;
import pt.tecnico.uilib.menus.CommandException;
import prr.core.exception.InvalidCommunicationException;

/**
 * Perform payment.
 */
class DoPerformPayment extends TerminalCommand {

  DoPerformPayment(Network context, Terminal terminal) {
    super(Label.PERFORM_PAYMENT, context, terminal);
    addIntegerField("id",Message.commKey());
  }
  
  @Override
  protected final void execute() throws CommandException {
    int id = (int) integerField("id");

    try{
      _network.payComm(id, _receiver);
    }
    catch(InvalidCommunicationException exc){
      _display.popup(Message.invalidCommunication());
    }
  }
}
