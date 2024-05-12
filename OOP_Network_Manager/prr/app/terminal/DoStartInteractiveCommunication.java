package prr.app.terminal;

import prr.core.Network;
import prr.core.terminal.*;
import prr.core.exception.*;
import prr.core.communications.CommNames;
import prr.app.exception.UnknownTerminalKeyException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.CommandException;

/**
 * Command for starting communication.
 */
class DoStartInteractiveCommunication extends TerminalCommand {

  DoStartInteractiveCommunication(Network context, Terminal terminal) {
    super(Label.START_INTERACTIVE_COMMUNICATION, context, terminal, receiver -> receiver.canStartCommunication());
    addStringField("id", Message.terminalKey());
    addOptionField("type",Message.commType(), 
                    CommNames.VOICE.toString(),CommNames.VIDEO.toString());
  }
  
  @Override
  protected final void execute() throws CommandException {
    String id = stringField("id");
    String type=stringField("type");
    try{
      _network.newInteractiveComm(_receiver, id, type);
    }
    catch(UnknownTerminalKeyExceptionCore exc){
      throw new UnknownTerminalKeyException(exc.getKey());
    }
    catch(DestinationIsSilentException exc){
      _display.popup(Message.destinationIsSilent(exc.getKey()));
    }
    catch(DestinationIsOffException exc){
      _display.popup(Message.destinationIsOff(exc.getKey()));
    }
    catch(DestinationIsBusyException exc){
      _display.popup(Message.destinationIsBusy(exc.getKey()));
    }
    catch(UnsupportedAtOriginException exc){
      _display.popup(Message.unsupportedAtOrigin(exc.getKey(),exc.getType()));
    }
    catch(UnsupportedAtDestinationException exc){
      _display.popup(Message.unsupportedAtDestination(exc.getKey(),exc.getType()));
    }
  }
}
