// Target class (position and width)
class Target
{
  constructor(x, y, w, l, id, colorid)
  {
    this.x      = x;
    this.y      = y;
    this.width  = w;
    this.label  = l;
    this.id     = id;
    this.labelFontSize=findMaxFontSize(l, w);
  }
  
  // Checks if a mouse click took place
  // within the target
  clicked(mouse_x, mouse_y)
  { 
    //if(this.id<=-10) return false;
    
    if(((this.x-this.width/2) > mouse_x)) return false;
    if(((this.x+this.width/2) < mouse_x)) return false;
    if(((this.y-this.width/2) > mouse_y)) return false;
    if(((this.y+this.width/2) < mouse_y)) return false;
    return true;
  }
  
  
  setCoordinates(x, y){
    this.x=x;
    this.y=y;
  }

  // Draws the target (i.e., a circle)
  // and its label
  draw()
  { 
    //if(this.id<=-10) return false;

    // Draw target
    if(this.id==-10){ //id==-10 goBackButton
      fill(color(64,64,64))
      if(keyCounter.label!="2ª"&&this.clicked(mouseX,mouseY)) fill(color(128,128,128))
      circle(this.x, this.y,this.width)
    }
    else if(this.id==-11){ //id==-11 keyCounter
      textFont("Arial", this.labelFontSize);
      textStyle(BOLD);
      fill(color(64,64,64));
      textAlign(CENTER,CENTER);
      text(this.label, this.x, this.y);
      return;
    }
    else{
      fill(color(255,255,255))
      if(this.clicked(mouseX,mouseY)) fill(color(222,222,222))
      rectMode(CENTER)
      rect(this.x, this.y, this.width, this.width);
    }
    // Draw label
    textFont("Arial", this.labelFontSize);
    textStyle(BOLD);
    fill(color(0,0,0));
    textAlign(CENTER,CENTER);
    text(this.label, this.x, this.y);
  }

  setLabel(l){
    this.label=l;
  }

}

function findMaxFontSize(text, maxWidth) {
  let size = 64;
  if(display_size<16) maxWidth=maxWidth*0.85;
  textSize(size);
  let currentWidth = textWidth(text);

  while (currentWidth > maxWidth) {
    size -= 1;
    textSize(size);
    currentWidth = textWidth(text);
  }

  return size;
}
