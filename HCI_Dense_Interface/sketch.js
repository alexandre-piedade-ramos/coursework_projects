// Bake-off #2 -- Seleção em Interfaces Densas
// IPM 2023-24, Período 3
// Entrega: até às 23h59, dois dias úteis antes do sexto lab (via Fenix)
// Bake-off: durante os laboratórios da semana de 18 de Março

// p5.js reference: https://p5js.org/reference/

// Database (CHANGE THESE!)
const GROUP_NUMBER        = 36;      // Add your group number here as an integer (e.g., 2, 3)
const RECORD_TO_FIREBASE  = true;  // Set to 'true' to record user results to Firebase

// Pixel density and setup variables (DO NOT CHANGE!)
let PPI, PPCM;
const NUM_OF_TRIALS       = 12;     // The numbers of trials (i.e., target selections) to be completed
let continue_button;
let legendas;                       // The item list from the "legendas" CSV

// Metrics (DO NOT CHANGE!)
let testStartTime, testEndTime;     // time between the start and end of one attempt (8 trials)
let hits 			      = 0;      // number of successful selections
let misses 			      = 0;      // number of missed selections (used to calculate accuracy)
let database;                       // Firebase DB  

// Study control parameters (DO NOT CHANGE!)
let draw_targets          = false;  // used to control what to show in draw()
let trials;                         // contains the order of targets that activate in the test
let current_trial         = 0;      // the current trial number (indexes into trials array above)
let attempt               = 0;      // users complete each test twice to account for practice (attemps 0 and 1)

// Target list and layout variables
let legendasArr           = [];
let targets_menu          = [];
let targets_menu_l        = {};
let groupedTargets        = [];
let keyCounter;
let goBackButton;
let ak1  = null;
let ak2 = null;
const GRID_ROWS           = 8;      // We divide our 80 targets in a 8x10 grid
const GRID_COLUMNS        = 10;     // We divide our 80 targets in a 8x10 grid

let rulesRead=false;

// Ensures important data is loaded before the program starts
function preload()
{
  // id,name,...
  legendas = loadTable('legendas.csv', 'csv', 'header');
}

// Runs once at the start
function setup()
{
  createCanvas(700, 500);    // window size in px before we go into fullScreen()
  frameRate(60);             // frame rate (DO NOT CHANGE!)
  
  randomizeTrials();         // randomize the trial order at the start of execution
  drawUserIDScreen();        // draws the user start-up screen (student ID and display size)
  legendasArr=legendas.getArray();
  
  soundCorrect   = loadSound('correctV2.ogg');
  soundIncorrect = loadSound('incorrectV2.ogg');
  //print(soundCorrect.setVolume())
  soundCorrect.setVolume(0.8)
  soundIncorrect.setVolume(0.8)
  rulesImg=loadImage("regras.png");
}

// Runs every frame and redraws the screen
function draw()
{
  if (draw_targets && attempt < 2)
  {     
    // The user is interacting with the 6x3 target grid
    background(color(0,0,0));        // sets background to black
    // Print trial count at the top left-corner of the canvas
    textFont("Arial", 16);
    fill(color(255,255,255));
    textAlign(LEFT);
    text("Trial " + (current_trial + 1) + " of " + trials.length, 50, 20);
    
    if((!ak1 && !ak2))  keyCounter.setLabel("1")
    else if((ak1 && !ak2))  keyCounter.setLabel("2")
    else keyCounter.setLabel("3")
    keyCounter.draw();
    goBackButton.draw();
    if(!ak1)
      Object.keys(targets_menu).forEach(idk=>targets_menu[idk].draw());

    else if(ak1 && !ak2)
      Object.keys(targets_menu_l[ak1]).forEach(idk=>targets_menu_l[ak1][idk].draw());

    else if(ak1 && ak2)
      groupedTargets[ak1][ak2].forEach(idk=>idk.draw());

    // Draws the target label to be selected in the current trial. We include 
    // a black rectangle behind the trial label for optimal contrast in case 
    // you change the background colour of the sketch (DO NOT CHANGE THESE!)
    fill(color(0,0,0));
    rect(0, height - 40, width, 40);
 
    textFont("Arial", 20); 
    fill(color(255,255,255)); 
    textAlign(CENTER); 
    text(legendas.getString(trials[current_trial],1), width/2, height - 20);
  }
}

// Print and save results at the end of 54 trials
function printAndSavePerformance()
{
  // DO NOT CHANGE THESE! 
  let accuracy			= parseFloat(hits * 100) / parseFloat(hits + misses);
  let test_time         = (testEndTime - testStartTime) / 1000;
  let time_per_target   = nf((test_time) / parseFloat(hits + misses), 0, 3);
  let penalty           = constrain((((parseFloat(95) - (parseFloat(hits * 100) / parseFloat(hits + misses))) * 0.2)), 0, 100);
  let target_w_penalty	= nf(((test_time) / parseFloat(hits + misses) + penalty), 0, 3);
  let timestamp         = day() + "/" + month() + "/" + year() + "  " + hour() + ":" + minute() + ":" + second();
  
  textFont("Arial", 18);
  background(color(0,0,0));   // clears screen
  fill(color(255,255,255));   // set text fill color to white
  textAlign(LEFT);
  text(timestamp, 10, 20);    // display time on screen (top-left corner)
  
  textAlign(CENTER);
  text("Attempt " + (attempt + 1) + " out of 2 completed!", width/2, 60); 
  text("Hits: " + hits, width/2, 100);
  text("Misses: " + misses, width/2, 120);
  text("Accuracy: " + accuracy + "%", width/2, 140);
  text("Total time taken: " + test_time + "s", width/2, 160);
  text("Average time per target: " + time_per_target + "s", width/2, 180);
  text("Average time for each target (+ penalty): " + target_w_penalty + "s", width/2, 220);

  // Saves results (DO NOT CHANGE!)
  let attempt_data = 
  {
        project_from:       GROUP_NUMBER,
        assessed_by:        student_ID,
        test_completed_by:  timestamp,
        attempt:            attempt,
        hits:               hits,
        misses:             misses,
        accuracy:           accuracy,
        attempt_duration:   test_time,
        time_per_target:    time_per_target,
        target_w_penalty:   target_w_penalty,
  }
  
  // Sends data to DB (DO NOT CHANGE!)
  if (RECORD_TO_FIREBASE)
  {
    // Access the Firebase DB
    if (attempt === 0)
    {
      firebase.initializeApp(firebaseConfig);
      database = firebase.database();
    }
    
    // Adds user performance results
    let db_ref = database.ref('G' + GROUP_NUMBER);
    db_ref.push(attempt_data);
  }
}

// Mouse button was pressed - lets test to see if hit was in the correct target
function mousePressed() 
{
  // Only look for mouse releases during the actual test
  // (i.e., during target selections)
  if(!draw_targets) return;

  if(goBackButton.clicked(mouseX, mouseY)){
    if(ak1 && ak2) ak2=null;
    else if(ak1 && !ak2) ak1=null;
    return;
  }
  // Check if the user clicked over one of the targets
  if(!ak1){
    Object.keys(targets_menu).forEach(
      idk=>{
        if(targets_menu[idk].clicked(mouseX, mouseY)){
          ak1=idk;
          ak2=null;
          return true;
        }
      }
    )
  }
  else if(ak1 && !ak2){
    Object.keys(targets_menu_l[ak1]).some(idk=>{
        if(targets_menu_l[ak1][idk].clicked(mouseX, mouseY)){
          ak2=idk;
          if(groupedTargets[ak1][idk].length==1){
            targetNameChosen(groupedTargets[ak1][idk][0])
          }
          return true;
        }
      }
    );
  }
  else if(ak1 && ak2){
    groupedTargets[ak1][ak2].some(idk=>{
        if(idk.clicked(mouseX, mouseY)){
            // Checks if it was the correct target
            targetNameChosen(idk);
            return true;
        }
      }
    )
  }
  
}

function targetNameChosen(t){
  //print(t,':',t.id,':',trials[current_trial]+1)
  if (t.id === trials[current_trial]+1){
    hits++;
    soundCorrect.play();
  }
  else{
    misses++;
    soundIncorrect.play();
  }
  current_trial++;              // Move on to the next trial/target
  ak1=null;
  ak2=null;
  // Check if the user has completed all trials
  if (current_trial === NUM_OF_TRIALS)
  {
    testEndTime = millis();
    draw_targets = false;          // Stop showing targets and the user performance results
    printAndSavePerformance();     // Print the user's results on-screen and send these to the DB
    attempt++;                      
    
    // If there's an attempt to go create a button to start this
    if (attempt < 2)
    {
      continue_button = createButton('START 2ND ATTEMPT');
      continue_button.mouseReleased(continueTest);
      continue_button.position(width/2 - continue_button.size().width/2, height/2 - continue_button.size().height/2);
    }
  }
  // Check if this was the first selection in an attempt
  else if (current_trial === 1) testStartTime = millis(); 
}
// Evoked after the user starts its second (and last) attempt
function continueTest()
{
  // Re-randomize the trial order
  randomizeTrials();
  
  // Resets performance variables
  hits = 0;
  misses = 0;
  
  current_trial = 0;
  continue_button.remove();
  
  // Shows the targets again
  draw_targets = true; 
}


// Creates and positions the UI targets
function createTargets(target_size, horizontal_gap, vertical_gap){
  // Define the margins between targets by dividing the white space 
  // for the number of targets minus one
  h_margin = horizontal_gap / (GRID_COLUMNS -1);
  v_margin = vertical_gap / (GRID_ROWS - 1);
  sortAndGroup(target_size);
  
  let allchars=[]
  legendas.getColumn('city').forEach(idk=>{
      for(i=0;i<idk.length;i++) allchars[unidecode(idk.charAt(i).toLowerCase())]=1;
    }
  )
  //allow for the menu groupings to start as close to the subtile as possible
  let floorR= GRID_ROWS-1-ceil(Object.keys(allchars).length/8)
  let floorC=3
  let coords=[]
  let coordsIndex=0;
  for (var r = floorR-1; r < GRID_ROWS-1; r++)
  {
    for (var c = floorC; c < GRID_COLUMNS-3; c++)
    { 
      let target_x = 40 + (h_margin + target_size) * c + target_size/2;        // give it some margin from the left border
      let target_y = 40 + (v_margin + target_size) * r + target_size/2;
      coords[coords.length]=[target_x,target_y]
    }
  }

  goBackButton=new Target(coords[0][0]+h_margin,coords[0][1]-h_margin,target_size,"<",-10,);
  keyCounter=new Target(coords[3][0]-h_margin,coords[3][1]-h_margin,target_size,"1",-11);

  for(var i=6; i<coords.length;i=i+7){
    var temp;
    if((i+3)>=coords.length) break;
    temp=coords[i][0];
    coords[i][0]=coords[i+2][0];
    coords[i+2][0]=temp;

    temp=coords[i][1];
    coords[i][1]=coords[i+2][1];
    coords[i+2][1]=temp;
    i++;
    temp=coords[i][0];
    coords[i][0]=coords[i+2][0];
    coords[i+2][0]=temp;

    temp=coords[i][1];
    coords[i][1]=coords[i+2][1];
    coords[i+2][1]=temp;
  }
  /*
  let vSeparation = ()=>{
                          var idk=floor(count / 8) * h_margin;
                          if(display_size>16 && count%4>=2) idk =  -1*coords[coordsIndex-1][1] + coords[coordsIndex-3][1]+target_size+1;
                          
                          else idk=0
                          return idk;
                        }
  */
  let vSeparation = ()=>{
    idk=1
    if(count%4>=2) idk+=  -1*coords[coordsIndex-1][1] + coords[coordsIndex-3][1]+target_size+1;
    if(display_size<=16) idk+=floor(count / 8) * h_margin;
    return idk;
  }
  let hSeparation = ()=>{
                          var modx,idk=1;
                          switch(count%8){
                            case 1: case 3: case 4: case 6: modx  = 0;  break;
                            case 0: case 2:                 modx  = 1;  break;
                            case 5: case 7:                 modx  =-1;  break;    
                          }
                          //if(display_size>16 && count%4 < 2) idk=h_margin-target_size
                          return modx*h_margin + count%2 * idk;
                        }
  let count = 0
  coordsIndex=4
  Object.keys(groupedTargets).forEach(
    k1=>{
      
      let newTarget = new Target(
                                  coords[coordsIndex][0]+hSeparation(),
                                  coords[coordsIndex++][1]+vSeparation(),
                                  target_size,
                                  k1,
                                  -1,
                                );
      targets_menu[k1]=newTarget;
      count++;
    }
  )

  count = 0
  coordsIndex=4
  Object.keys(groupedTargets).forEach(
    k1=>{
      count = 0
      coordsIndex=4
      Object.keys(groupedTargets[k1]).forEach(
        k2=>{
          let newTarget = new Target(
                                      coords[coordsIndex][0]+hSeparation(),
                                      coords[coordsIndex++][1]+vSeparation(),
                                      target_size,
                                      k2,
                                      -2,
                                    );
          if (!(k1 in targets_menu_l)) {
            targets_menu_l[k1] = [];
          }
          targets_menu_l[k1][k2]=newTarget;
          count++;
        }
      )
    }
  )
  
  count = 0
  coordsIndex=4
  Object.keys(groupedTargets).forEach(
    k1=>{
      count = 0
      coordsIndex=4
      Object.keys(groupedTargets[k1]).forEach(
        k2=>{
          count = 0
          coordsIndex=4
          groupedTargets[k1][k2].forEach(
            t=>{
              t.setCoordinates(coords[coordsIndex][0]+hSeparation(),
                               coords[coordsIndex++][1]+vSeparation()
                              );
              count++
            }
          )
        }
      )
    }
  )
  

    //print("groupedTargets: ",groupedTargets)
    //print("targets_menu: ",targets_menu)
    //print("targets_menu_l: ",targets_menu_l)


}

let resizedOnce=false;
// Is invoked when the canvas is resized (e.g., when we go fullscreen)
function windowResized() 
{ 
  if(resizedOnce) return;
  if (fullscreen() && rulesRead)
  {
    resizeCanvas(windowWidth, windowHeight);
    
    // DO NOT CHANGE THE NEXT THREE LINES!
    let display        = new Display({ diagonal: display_size }, window.screen);
    PPI                = display.ppi;                      // calculates pixels per inch
    PPCM               = PPI / 2.54;                       // calculates pixels per cm
  
    // Make your decisions in 'cm', so that targets have the same size for all participants
    // Below we find out out white space we can have between 2 cm targets
    let screen_width   = display.width * 2.54;             // screen width
    let screen_height  = display.height * 2.54;            // screen height
    let target_size    = 2;                                // sets the target size (will be converted to cm when passed to createTargets)
    let horizontal_gap = screen_width - target_size * GRID_COLUMNS;// empty space in cm across the x-axis (based on 10 targets per row)
    let vertical_gap   = screen_height - target_size * GRID_ROWS;  // empty space in cm across the y-axis (based on 8 targets per column)

    // Creates and positions the UI targets according to the white space defined above (in cm!)
    // 80 represent some margins around the display (e.g., for text)
    createTargets(target_size * PPCM, horizontal_gap * PPCM - 80, vertical_gap * PPCM - 80);

    // Starts drawing targets immediately after we go fullscreen
    draw_targets = true;
    resizedOnce  = true;
  }
}

function sortAndGroup(target_size) {
  // Sort the words
  legendasArr.sort(legendasArrCmp);
  // Group the words by their first 2 characters
  legendasArr.forEach(target => {
      let k1 = unidecode(target[1].substring(1,2));
      let k2 = unidecode(target[1].substring(2,3));
      if (!(k1 in groupedTargets)) {
        groupedTargets[k1] = [];
      }
      if (!(k2 in groupedTargets[k1])) {
          groupedTargets[k1][k2] = [];
        }
      let newTarget=new Target(0, 0, target_size, target[1], int(target[0]), 0);
      groupedTargets[k1][k2].push(newTarget);
  });
  return groupedTargets;
}
function unidecode(str){
  str=str.replace("á","a");
  str=str.replace("ł","l");
  str=str.replace("é","e");
  str=str.replace("ș","s");
  str=str.replace("š","s");
  str=str.replace("í","i");
  return str;
}

function legendasArrCmp(a,b){
  let c=a[1];
  let d=b[1];
  c=unidecode(c);
  d=unidecode(d);
  if(c>d)  return 1;
  if(c<d)  return -1;
  return 0;
}