package prr.app.terminal;

import prr.core.Network;
import prr.core.terminal.*;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.CommandException;

/**
 * Command for ending communication.
 */
class DoEndInteractiveCommunication extends TerminalCommand {

  DoEndInteractiveCommunication(Network context, Terminal terminal) {
    super(Label.END_INTERACTIVE_COMMUNICATION, context, terminal, receiver -> receiver.canEndCurrentCommunication());
    addIntegerField("duration", Message.duration());
  }
  
  @Override
  protected final void execute() throws CommandException {
    Integer duration = integerField("duration");
    
    _display.popup(Message.communicationCost((long) Math.round(_receiver.endOngoingCall(duration))));
    
  }
}
