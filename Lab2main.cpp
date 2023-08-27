// Adjusted from and refered to:  Copyright (c) Donald Dansereau, 2023
// Adjusted and Added on from Lab 1 tasks:

//--Includes-------------------------------------------------------------------
#include <iostream>
#include <string>



//--Consts and enums-----------------------------------------------------------
const int InputsPerGate = 2; // number of inputs per logic gate
const int MaxFanout = 2;     // maximum fanout: max gate inputs that one gate output can drive

enum eLogicLevel             // enum defining the possible states of a logic line
{
  LOGIC_UNDEFINED = -1,
  LOGIC_LOW,
  LOGIC_HIGH
};



//---Base class for all logic gates--------------------------------------------
class LogicGate
{
  public:
  virtual void Init();                                            // Initialise the logic gate
  virtual void ConnectOutput(CWire* apOutputConnection);          // 
  virtual void DriveInput(int aInputIndex, eLogicLevel aNewLevel);// Drive an input with a logic level
  virtual void ComputeOutput() = 0;                               // Compute the output based on current input status
  virtual eLogicLevel GetOutputState();                           // Get the current output status


  protected:
  eLogicLevel mInputs[ InputsPerGate ]; // Inputs to Logic gate
  eLogicLevel mOutputValue;             // Current output value
  CWire* mpOutputConnection;            // Connections to output wire  

};



//---CWire Interface-----------------------------------------------------------
// CWire is used to connect devices in this simulation
// A CWire has a single input, and may drive multiple outputs
// The global variable MaxFanout controls how many outputs each wire can have
// Each wire output drives a specific input of a specific gate
// The wire's input is controlled via the DriveLevel function
class CWire
{
  public:
    void Init();
    
    // AddOutputConnection adds to the list of outputs that this wire drives
    // It accepts as parameters the logic gate whose input should be driven
    // and the index specifying which of that gate's inputs should be driven.
    void AddOutputConnection( LogicGate* apGateToDrive, int aGateInputToDrive );
    
    // DriveLevel drives the wire's value, so that each of its connected outputs
    // get set to the corresponding level
    void DriveLevel( eLogicLevel aNewLevel );
    
  private:
    int mNumOutputConnections;            // how many outputs are connected
    LogicGate* mpGatesToDrive[MaxFanout]; // list of connected gates
    int mGateInputIndices[MaxFanout];     // list of input to drive in each gate
};



//---Logic Gates Interface-------------------------------------------------------
// NAND
class CNandGate : public LogicGate
{
  public:
    // Compute the output based on current input status
    void ComputeOutput() override;
};

// AND
class CAndGate : public LogicGate
{
public:
    void ComputeOutput() override;
};

//OR
class COrGate : public LogicGate
{
public:
    void ComputeOutput() override;
};

//XOR
class CXorGate : public LogicGate
{
public:
    void ComputeOutput() override;
};


//---Logic structure difin-----------------------------------------------------
class CLogic
{
  public:
    void Init();    // Initialise the Logic circuit by connecting wires
    void Outcome(); // Test and print the logic circuit outcomes

  private:
    static const int NumlogicGates = 3; // number of logic gates
    static const int NumWires = 4;     // number of wires
    CAndGate MyGates[NumlogicGates];   // Array of logic gates
    CWire MyWires[NumWires];           // Arrat of wires
};



//---main----------------------------------------------------------------------
int main()
{
  CLogic Logic;     // Create a CLogic Class
  Logic.Init();     // Initialize the logic
  Logic.Outcome();  // Test and print the outcomes

  return 0;
}



//---CLogic Implementation-----------------------------------------------------
void CLogic::Init()
{
  
  // Initialize all logic gates
  for ( int i=0; i<NumlogicGates; ++i )
  {
    MyGates[i].Init();
  }

  // Initialize all wires
  for ( int i=0; i<NumWires; ++i )
  {
    MyWires[i].Init();
  }

  // Set up connections of logic gates to creat an OR gate
  // MyWires[0] and [1] are input wires for the circuit
  // MyWires[2] is a connection in between first two gates to the third gate
  MyWires[0].AddOutputConnection( &MyGates[0], 0 );
  MyWires[0].AddOutputConnection( &MyGates[0], 1 );
  MyWires[1].AddOutputConnection( &MyGates[1], 0 );
  MyWires[1].AddOutputConnection( &MyGates[1], 1 );
  MyWires[2].AddOutputConnection( &MyGates[2], 0 );
  MyWires[3].AddOutputConnection( &MyGates[2], 1 );

  // Output from gate 1 and gate 2 which use as input of the third gate
  MyGates[0].ConnectOutput( &MyWires[2] );
  MyGates[1].ConnectOutput( &MyWires[3] );

}

//---
void CLogic::Outcome()
{
  // Iterate through the possible input combinations
  for (int i = 0; i <= 1; ++i) {
      for (int j = 0; j <= 1; ++j) {
          eLogicLevel input1 = i ? LOGIC_HIGH : LOGIC_LOW;
          eLogicLevel input2 = j ? LOGIC_HIGH : LOGIC_LOW;
          
          // Input logic high and logic low to the circuit
          MyWires[0].DriveLevel(input1);
          MyWires[1].DriveLevel(input2);
          
          // Print the testing result
          std::cout << "Testing input " << i << " " << j
                    << " result: " << MyGates[2].GetOutputState() << std::endl;
      }
  }
}



//---LogicGate Base Implementation---------------------------------------------
void LogicGate::Init()
{
  mInputs[0] = mInputs[1] = LOGIC_UNDEFINED;
  mpOutputConnection = NULL;
  ComputeOutput();
}

//
void LogicGate::ConnectOutput(CWire* apOutputConnection)
{
  mpOutputConnection = apOutputConnection;
}

//
void LogicGate::DriveInput(int aInputIndex, eLogicLevel aNewLevel)
{
  mInputs[aInputIndex] = aNewLevel;
  ComputeOutput();  // Compute the output based on current input
}

//
void LogicGate::ComputeOutput()
{

}

//
eLogicLevel LogicGate::GetOutputState()
{
  return mOutputValue; 
}



//---CWire Implementation------------------------------------------------------
void CWire::Init()
{
  mNumOutputConnections = 0; // Initialize with no connection
}

// Add a new output connection to the wire
// apGateTo Drive: point to the logic gate to be driven
// aGateInputToDrive: the input index of the gate
void CWire::AddOutputConnection( LogicGate* apGateToDrive, int aGateInputToDrive )
{
  mpGatesToDrive[mNumOutputConnections] = apGateToDrive;
  mGateInputIndices[mNumOutputConnections] = aGateInputToDrive;
  ++mNumOutputConnections; // Increase number of output connections
}

// Drive the wire with a specific logic level
// aNewLevel: the new logic level through the wire
void CWire::DriveLevel( eLogicLevel aNewLevel )
{
  for ( int i=0; i<mNumOutputConnections; ++i )
    mpGatesToDrive[i]->DriveInput( mGateInputIndices[i], aNewLevel );
}



//---Logic Gates Implementation--------------------------------------------------
// NAND
// Computes the output of the NAND gate based on its current input values
void CNandGate::ComputeOutput()
{
  // Default is high
  eLogicLevel NewVal = LOGIC_HIGH; 

  // Undefined if input is undefined
  if ( mInputs[0] == LOGIC_UNDEFINED || mInputs[1] == LOGIC_UNDEFINED )
  {
    NewVal = LOGIC_UNDEFINED;  
  }

  // Low if both input are high
  else if ( mInputs[0] == LOGIC_HIGH && mInputs[1] == LOGIC_HIGH )
  { 
    NewVal = LOGIC_LOW;            
  }

  mOutputValue = NewVal; //The new output value

  // output the value to the connected wire if there is one
  if ( mpOutputConnection != NULL )
  {
    mpOutputConnection->DriveLevel( mOutputValue );
  }
}

//AND
void CAndGate::ComputeOutput()
{
  // Default is undifined
  eLogicLevel mOutputValue = LOGIC_UNDEFINED;

  // High if both inputs are high
  if ( mInputs[0] == LOGIC_HIGH && mInputs[1] == LOGIC_HIGH )
  {
    mOutputValue = LOGIC_HIGH;
  }

  // Else low
  else
  {
    mOutputValue = LOGIC_LOW;
  }

  // output the value to the connected wire if there is one
  if ( mpOutputConnection != NULL )
  {
    mpOutputConnection->DriveLevel( mOutputValue );
  }
}

//OR
void COrGate::ComputeOutput()
{
  // Default is undifined
  eLogicLevel mOutputValue = LOGIC_UNDEFINED;

  // High if at least one inputs are high
  if ( mInputs[0] == LOGIC_HIGH || mInputs[1] == LOGIC_HIGH )
  {    
    mOutputValue = LOGIC_HIGH;
  }

  // Else low
  else
  {
    mOutputValue = LOGIC_LOW;
  }

  // output the value to the connected wire if there is one
  if ( mpOutputConnection != NULL )
  {
    mpOutputConnection->DriveLevel( mOutputValue );
  }
}

//XOR
void CXorGate::ComputeOutput()
{
  // Default is undifined
  eLogicLevel mOutputValue = LOGIC_UNDEFINED;

  // LOW if both inputs are high
  if ((mInputs[0] == LOGIC_HIGH && mInputs[1] == LOGIC_HIGH) || 
      (mInputs[0] == LOGIC_LOW  && mInputs[1] == LOGIC_LOW ))
  {    
    mOutputValue = LOGIC_LOW;
  }

  // Else high if only one is high
  else if ( mInputs[0] == LOGIC_HIGH || mInputs[1] == LOGIC_HIGH )
    mOutputValue = LOGIC_HIGH;

  // output the value to the connected wire if there is one
  if ( mpOutputConnection != NULL )
  {
    mpOutputConnection->DriveLevel( mOutputValue );
  }
}

