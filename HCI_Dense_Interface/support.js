// Support variables & functions (DO NOT CHANGE!)

let student_ID_form, display_size_form, start_button;                  // Initial input variables
let student_ID, display_size;                                          // User input parameters

// Prints the initial UI that prompts that ask for student ID and screen size
function drawUserIDScreen()
{ 
  background(color(0,0,0));                                          // sets background to black
  
  // Text prompt
  main_text = createDiv("Insert your student number and display size");
  main_text.id('main_text');
  main_text.position(10, 10);
  
  // Input forms:
  // 1. Student ID
  let student_ID_pos_y_offset = main_text.size().height + 40;         // y offset from previous item
  
  student_ID_form = createInput('');                                 // create input field
  student_ID_form.position(200, student_ID_pos_y_offset);
  
  student_ID_label = createDiv("Student number (int)");              // create label
  student_ID_label.id('input');
  student_ID_label.position(10, student_ID_pos_y_offset);
  
  // 2. Display size
  let display_size_pos_y_offset = student_ID_pos_y_offset + student_ID_form.size().height + 20;
  
  display_size_form = createInput('');                              // create input field
  display_size_form.position(200, display_size_pos_y_offset);
  
  display_size_label = createDiv("Display size in inches");         // create label
  display_size_label.id('input');
  display_size_label.position(10, display_size_pos_y_offset);
  
  // 3. Rules button
  rules_button = createButton('Show Rules');
  rules_button.mouseReleased(showRulesScreen);
  rules_button.size(150,40);
  rules_button.style('font-size', '24px');
  rules_button.position(width / 2 - rules_button.size().width / 2, height - (rules_button.size().height) * 2);

  textFont("Arial", 24);
  textStyle(BOLD);
  fill(color(255,255,255));
  textAlign(CENTER,CENTER);
  text("Recomendamos 14 polegadas como display size", rules_button.x+rules_button.width/2, rules_button.y-60);
}

// Verifies if the student ID is a number, and within an acceptable range
function validID()
{
  if(parseInt(student_ID_form.value()) < 200000 && parseInt(student_ID_form.value()) > 1000) return true
  else 
  {
    alert("Please insert a valid student number (integer between 1000 and 200000)");
	return false;
  }
}

// Verifies if the display size is a number, and within an acceptable range (>13")
function validSize()
{
  if (parseInt(display_size_form.value()) < 50 && parseInt(display_size_form.value()) >= 13) return true
  else
  {
    alert("Please insert a valid display size (between 13 and 50)");
    return false;
  }
}

// Starts the test (i.e., target selection task)
function startTest()
{
  if (validID() && validSize())
  {
      rulesRead = true;
      start_button.remove();
      clear();
      windowResized();
  }
}

// Randomize the order in the targets to be selected
function randomizeTrials()
{
  trials = [];      // Empties the array
    
  // Creates an array with random items from the "legendas" CSV
  for (var i = 0; i < NUM_OF_TRIALS; i++) trials.push(floor(random(legendas.getRowCount())));

  print("trial order: " + trials);   // prints trial order - for debug purposes
}

// Function to show the rules screen
function showRulesScreen() {
  
  if (validID() && validSize())
  {
    
    // Saves student and display information
    student_ID = parseInt(student_ID_form.value());
    display_size = parseInt(display_size_form.value());
    
    
    fullscreen(!fullscreen());
    //resizeCanvas(windowWidth, windowHeight);
    resizeCanvas(displayWidth, displayHeight);
    
    // Deletes UI elements
    main_text.remove();
    student_ID_form.remove();
    student_ID_label.remove();
    display_size_form.remove();
    display_size_label.remove();
    rules_button.remove();
    
  
    let rules_text= "Regras do Jogo:";
    textAlign(CENTER);
    fill(color(255, 255, 255));
    textFont("Arial", width / 40);
    text(rules_text, width/2, 40);
    
    // Alert message
    imageMode(CENTER)
    image(rulesImg, width/2, height/2, width*0.8,height*0.8);
    
    fill(color(0, 0, 0));
    // Create "Start" button on the rules screen
    start_button = createButton('START');
    start_button.mouseReleased(startTest);
    start_button.size(100,40);
    start_button.style('font-size', '24px');
    start_button.position(width/2 - start_button.size().width/2, height -75);
  }
}