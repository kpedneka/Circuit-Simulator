#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

//Hash function takes a char
//returns the index in the array it should be in
int hash(char name)
{
  int temp = -1;

  if (name <= 'Z' && name >= 'A')
    {
      temp = (int) name;
      return temp-65;
    }
  else if (name <= 'z' && name >= 'a')
    {
      temp = (int) name;
      return temp-71;
    }
  return temp;

}

//intializes an array with values only for a 2:4 DECODER
void decoder(int *greycode, int *table, char first, char second)
{
  int a, b, nota, notb;
  a = table[hash(first)];
  b = table[hash(second)];
  nota = !a;
  notb = !b;

  greycode[0] = nota&&notb;
  greycode[1] = nota&&b;
  greycode[2] = a&&b;
  greycode[3] = a&&notb;
  
}

//takes an array which already has decoded values and expands
//it for one more variable
void decode(int *greycode, int *table, int size, char first)
{
  int offset, temp;
  
  //mirror copy the values in the array
  for (offset = 0; offset < size/2; offset++)
    {
      temp = greycode[offset];
      greycode[size-1-offset] = temp;
    }

  int a, nota;
  a= table[hash(first)];
  nota = !a;

  //for the first half of the array, do !a&&<<whatever value>>
  for (offset = 0; offset < size/2; offset++)
    {
      temp  = greycode[offset];
      greycode[offset] = nota&&temp;
    }

  //for the second half of the array, do a&&<<whatever value>>
  for (offset = (size/2); offset < size; offset++)
    {
      temp = greycode[offset];
      greycode[offset] = a&&temp;
    }

}


void creategrey(int *arr)
{
  arr[0] = 0;
  arr[1] = 1;
}

void makegrey(int *arr, int size)
{
  int i;
  int end = size*2;
  for(i = 0; i < size; i++)
    {
      arr[end-1-i] = arr[i]+size; 
    }
}

//Operate does operations AND NOT OR DECODER MUX
void operate(int *table, FILE *circ)
{
  //These declarations are for the switch cases AND and OR
  int var1, var2, var3;
  char a, b, c;

  //Reading the operation name
  char temp[100];
  fscanf(circ, "%s", temp);

  switch (temp[0])
    {
      //Case AND
    case 'A': 
      fscanf(circ, " %c %c %c\n", &a, &b, &c);

      int hashA = hash(a);
      int hashB = hash(b);
      int hashC = hash(c);

      var1 = table[hashA];
      var2 = table[hashB];

      if (var1 == -1)
	var1 = atoi(&a);
      if (var2 == -1)
	var2 = atoi(&b);

      var3 = var1&&var2;
      //printf("AND: %d\n", var3);
      table[hashC] = var3;
      break;
      
      //Case OR
    case 'O': 
      fscanf(circ, " %c %c %c\n", &a, &b, &c);

      var1 = table[hash(a)];
      var2 = table[hash(b)];
      
      if (var1 == -1)
	var1 = atoi(&a);
      if (var2 == -1)
	var2 = atoi(&b);

      var3 = var1||var2;
      //printf("OR: %d\n", var3);
      table[hash(c)] = var3;
      break;

      //Case NOT
    case 'N':
      fscanf(circ, " %c %c\n", &a, &b);

      var1 = table[hash(a)];

      if (var1 == -1)
	var1 = atoi(&a);

      var2 = !var1;
      //printf("NOT: %d\n", var2);
      table[hash(b)] = var2;
      break;

      //Case DECODER
    case 'D': ;

      //Declarations of variables for this function's use
      int greycode[100], temp, size, counter = 0;
      char greyoutput[100];


      //Find out how many inputvars
      fscanf(circ, " %d ", &temp);

      //Reading the DECODER line and getting the input vars
      while(counter < temp)
	{
	  fscanf(circ, " %c", &a);
	  greyoutput[counter] = a;
	  counter++;
	}

      //Decode for last 2 variables
      char first, second;
      first = greyoutput[counter-2];
      second = greyoutput[counter-1];

      decoder(greycode, table, first, second);
      counter -= 2;
      size = 4;

      //If there are more than 2 input variables
      while(counter > 0)
	{
	  size = size*2;
	  decode(greycode, table, size, greyoutput[counter-1]);
	  counter--;
	}

      //put these values back in for variables
      temp = (int) pow(2.0, temp);      

      //Get output vars
      int loop;
      for (loop = 0; loop < temp; loop++)
	{
	  fscanf(circ, " %c", &a);
	  greyoutput[loop] = a;
	}
      fscanf(circ, "\n");
      //set values for the local variables in temp array
      while (loop >= 0)
	{
	  char ind = greyoutput[loop];
	  table[hash(ind)] = greycode[loop];
	  loop--;
	}
      break;

      //Case MULTIPLEXER
    case 'M': ;

      //int index, inpindex;
      char greyinps[100], selectors[100], output;
      counter = 0;

      //MUX INPUTS XOR OUTPUTS CAN BE VARIABLES OR INT VALUES
      fscanf(circ, " %d", &temp);


      //Scan input variables
      while (counter < temp)
	{
	  fscanf(circ, " %c", &a);
	  greyinps[counter] = a;
	  counter++;
	}

      int numselectors;
      //Scan output selectors
      numselectors = (int) log2(temp);


      for (counter = 0; counter < numselectors; counter++)
	{
	  fscanf(circ, " %c", &a);
	  selectors[counter] = a;
	}


      //Scan output variable
      fscanf(circ, " %c\n", &output);      
      
      //This is the array which contains greycode decimal
      //converted numbers
      int arr[1000];

      //This creates the array that contains the indexes
      creategrey(arr);
      size = 2;
      while (numselectors-2 >= 0)
	{
	  makegrey(arr, size);
	  size = size*2;
	  numselectors--;
	}      

      //Convert selector value to binary
      int index = 0;
      loop = counter-1;
      int power;

      while (loop >= 0)
	{
	  //if the selector has a variable value
	  if (hash(selectors[loop]) != -1)
	    {
	      char c = selectors[loop];
	      int val = hash(c);
	      if (table[val] == 1)
		{
		  power = counter-loop-1;
		  index += pow(2.0, power);
		}
	    }
	  //if the selector is a int number
	  else if (selectors[loop] == '1')
	    {
	      power = counter-loop-1;
	      index += pow(2.0, power);
	    }
	  loop--;
	}
      
      //find the index in arr where index = arr[ind]
      for (loop = 0; loop < size; loop++)
	{
	  if (arr[loop] == index)
	    {
	      index = loop;
	      break;
	    }
	}
      int val = hash(output);

      if (greyinps[index] == '1' || greyinps[index] == '0')
	{	  	  
	  char c = greyinps[index];
	  table[val] = atoi(&c);
	}
      else
	table[val] = table[hash(greyinps[index])];

      //printf("output:%c %d\n", output, table[val]);
      break;

    default:
      break;
    }

}

int main(int argc, char **argv)
{
  FILE *circ, *input;
  
  if (argc != 3)
    {
      printf("bad input\n");
      return 0;
    }

  circ = fopen(argv[1], "r");
  input = fopen(argv[2], "r");

  if (circ == NULL || input == NULL)
    {
      printf("bad file\n");
    }

  //These declarations are for setting up the variables
  //that are read from INPUTVAR and OUTPUTVAR and other
  //local variables that are needed
  char operation[100], var, temp;
  int index, val, numvars, counter = 0;

  while (1)
    {
      counter = 0;

      //Scanning INPUTVAR and the number
      fscanf(circ, "%s %d", operation, &numvars);


      //Creating an array ready to hash variable values into it
      int table[52];

      
      //While loop for initializing INPUTVAR values
      while (counter < numvars)
	{

	  fgetc(circ);
	  var = fgetc(circ);

	  temp = var;

	  if (fscanf(input, "%d", &val) != 1)
	    break;


	  //store this in an array
	  index = hash(var);
	  table[index] = val;
	  counter++;
	}
    
      //Scanning OUTPUTVAR line
      fscanf(circ, "%s %d", operation, &numvars);


      counter = 0;

      //this variable keeps track of outputvars
      //makes it easy when we have to print results
      char outputvars[100];

      //While loop for making note of which variables are 
      //output variables so we can print them at the end
      while (counter < numvars)
	{
	  fgetc(circ);
	  var = fgetc(circ);
 
	  temp = var;
	  outputvars[counter] = temp;

	  counter++;
	}

      if (feof(input))
	break;

      //Now we will deal with the operations AND OR NOT MUX DECODER
      while (1)
	{
	  operate(table, circ);
	  if (feof(circ))
	    break;
	}
  
      int ind;
      for (index = 0; index < counter; index++)
	{
	  ind = hash(outputvars[index]);
	  if (table[ind] != 1 && table[ind] != 0)
	    break;
	  
	  printf("%d ", table[ind]);
	}
      printf("\n");


      if ((var = fgetc(input)) == EOF)
	break; 
      else
	{
	  rewind(circ);	
	}
    }

  fclose(input);
  fclose(circ);

  return 0;
}
