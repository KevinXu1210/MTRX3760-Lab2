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

// OR
class COrGate : public LogicGate
{
public:
    void ComputeOutput() override;
};

// XOR
class CXorGate : public LogicGate
{
public:
    void ComputeOutput() override;
};



//---Logic circuit-------------------------------------------------------------
// A logic circuit is a collection of logic gates and wires
// The circuit is initialised by connecting the gates and wires together
// The circuit can then be tested by applying inputs and printing the outputs
//---HalfAdder Class----------------------------------------------------------
// A half adder is a logic circuit that adds two inputs
// It has two outputs: a sum and a carry
// The sum is computed by a XOR gate
// The carry is computed by an AND gate
class CHalfAdder
{
  public:
    void Init();
    void Compute(eLogicLevel a, eLogicLevel b);  // Compute the sum and carry based on inputs
    eLogicLevel Sum();                        // The sum output
    eLogicLevel Carry();                      // The carry output
  
  private:
    CXorGate SumGate;  // XOR gate for the sum output
    CAndGate CarryGate;// AND gate for the carry output
  
    CWire aInputWire;     // Input wires a
    CWire bInputWire;     // Input wires b
    CWire sumOutputWire;  // Output wire for the sum
    CWire carryOutputWire;// Output wire for the carry
};



//---FullAdder Class----------------------------------------------------------
// A full adder is a logic circuit that adds three inputs
// It has two outputs: a sum and a carry
// The sum is computed by a XOR gate
// The carry is computed by two AND gates and an OR gate
class CFullAdder : public CHalfAdder // Inheritance from half adder
{
  public:
    void Init();
    void Compute(eLogicLevel a, eLogicLevel b, eLogicLevel cin); // Additional cin input
    eLogicLevel GetCarry();  // Override the carry from CHalfAdder
  
  private:
    CHalfAdder firstHalf;  // Two half adders
    CHalfAdder secondHalf; 
    COrGate CarryOrGate;   // OR gate for the final carry output
  
    CWire cinWire;
    CWire interSumWire;     // Intermediate sum
    CWire firstCarryWire;   // Intermediate carry
    CWire secondCarryWire;  // Final carry
};



//---3BitAdder Class-----------------------------------------------------------
// A 3-bit adder is a logic circuit that adds two 3-bit inputs
// It has two outputs: a 3-bit sum and a carry
// The sum is computed by three full adders
// The carry is computed by two full adders
class C3BitAdder
{
  public:
    void Init();
    void Compute(eLogicLevel a[3], eLogicLevel b[3], eLogicLevel cin);  // Array inputs for a and b
  
  private:
    CFullAdder bitAdders[3];  // Array of full adders
};



//---Logic structure difin-----------------------------------------------------
class CLogic
{
  public:
    void Init();    // Initialise the Logic circuit by connecting wires
    void Outcome(); // Test and print the logic circuit outcomes

  private:
    static const int NumlogicGates = 3;// number of logic gates
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



//---HalfAdder Implementation--------------------------------------------------
void CHalfAdder::Init()
{
  // Initialize all wires
  aInputWire.Init(); 
  bInputWire.Init();
  sumOutputWire.Init();
  carryOutputWire.Init();

  // Initialize all gates
  SumGate.Init();
  CarryGate.Init();

  // Connect all wires to the gates
  aInputWire.AddOutputConnection(&SumGate, 0);
  bInputWire.AddOutputConnection(&SumGate, 1);
  
  aInputWire.AddOutputConnection(&CarryGate, 0);
  bInputWire.AddOutputConnection(&CarryGate, 1);
  
  // Connect all gates to the wires
  SumGate.ConnectOutput(&sumOutputWire);//
  CarryGate.ConnectOutput(&carryOutputWire);
}



// Set the input values of the half adder
void CHalfAdder::Compute(eLogicLevel a, eLogicLevel b)
{
  aInputWire.DriveLevel(a);
  bInputWire.DriveLevel(b);
}


// Get the sum and carry output of the half adder
eLogicLevel CHalfAdder::Sum()
{
  return sumOutputWire.DriveLevel;
}

eLogicLevel CHalfAdder::Carry()
{
  return carryOutputWire.DriveLevel;
}

//---FullAdder Implementation--------------------------------------------------
void CFullAdder::Init()
{
  CHalfAdder::Init();
  cinWire.Init();
  interSumWire.Init();
  firstCarryWire.Init();
  secondCarryWire.Init();
  
  firstHalf.Init();
  secondHalf.Init();
  CarryOrGate.Init();
  
  // First Half Adder's Input
  aInputWire.AddOutputConnection(&firstHalf, 0);
  bInputWire.AddOutputConnection(&firstHalf, 1);

  // Second Half Adder's Input
  cinWire.AddOutputConnection(&secondHalf, 0);
  interSumWire.AddOutputConnection(&secondHalf, 1);
  
  // OR Gate for Final Carry Output
  firstCarryWire.AddOutputConnection(&CarryOrGate, 0);
  secondCarryWire.AddOutputConnection(&CarryOrGate, 1);
}

void CFullAdder::Compute(eLogicLevel a, eLogicLevel b, eLogicLevel cin)
{
  firstHalf.Compute(a, b);
  interSumWire.DriveLevel(firstHalf.GetSum());
  secondHalf.Compute(interSumWire.DriveLevel, cin);
}

eLogicLevel CFullAdder::GetCarry()
{
  return CarryOrGate.GetOutputState();
}

//---3BitAdder Implementation--------------------------------------------------
void C3BitAdder::Init()
{
  for(int i=0; i<3; ++i)
  {
    bitAdders[i].Init();
  }
}

void C3BitAdder::Compute(eLogicLevel a[3], eLogicLevel b[3], eLogicLevel cin)
{
  bitAdders[0].Compute(a[0], b[0], cin);
  for(int i=1; i<3; ++i)
  {
    bitAdders[i].Compute(a[i], b[i], bitAdders[i-1].GetCarry());
  }
}
