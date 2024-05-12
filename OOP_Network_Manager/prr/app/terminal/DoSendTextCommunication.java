package prr.app.terminal;

import prr.core.Network;
import prr.core.terminal.*;
import prr.app.exception.UnknownTerminalKeyException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.CommandException;

import prr.core.exception.DestinationIsOffException;
import prr.core.exception.UnknownTerminalKeyExceptionCore;


/**
 * Command for sending a text communication.
 */
class DoSendTextCommunication extends TerminalCommand {

  DoSendTextCommunication(Network context, Terminal terminal) {
    super(Label.SEND_TEXT_COMMUNICATION, context, terminal, receiver -> receiver.canStartCommunication());
    addStringField("id", Message.terminalKey());
    addStringField("text", Message.textMessage());
  }
  
  @Override
  protected final void execute() throws CommandException, UnknownTerminalKeyException{
    String id = stringField("id");
    String text=stringField("text");
    try{
    _network.newTextComm(_receiver, id, text);
    }
    catch(UnknownTerminalKeyExceptionCore exc){
      throw new UnknownTerminalKeyException(exc.getKey());
    }
    catch(DestinationIsOffException exc){
      _display.popup(Message.destinationIsOff(exc.getKey()));
    }
  }
} 
