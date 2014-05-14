module IRDA_RECEIVE_Terasic(iCLK, iRST_n, iIRDA, iREAD, oDATA_REAY, oDATA);

input iCLK, iRST_n, iIRDA, iREAD;
output oDATA_REAY, oDATA;
                   
parameter IDLE = 2'b00;         
parameter GUIDANCE = 2'b01;   
parameter DATAREAD = 2'b10;

parameter IDLE_HIGH_DUR      = 262143;
parameter GUIDE_LOW_DUR      = 230000;
parameter GUIDE_HIGH_DUR     = 210000;
parameter DATA_HIGH_DUR      = 41500;
parameter BIT_AVAILABLE_DUR  = 20000;

reg DATA_REAY;
reg [17:0] idle_count; 
reg idle_count_flag; 
reg [17:0] state_count;
reg state_count_flag; 
reg [17:0] data_count; 
reg data_count_flag;
reg [5:0] bitcount;
reg [1:0] state; 
reg [31:0] DATA; 
reg [31:0] DATA_BUF; 
reg [31:0] oDATA; 

initial
    begin
        state =  IDLE;     //state initial
        DATA_REAY = 1'b0;  //set the dataready flag to 0
        bitcount = 0;      //set the data reg pointer to 0
        DATA = 0;          //clear the data reg
        DATA_BUF = 0;      //clear the data buf
        state_count = 0;   //guide high level time counter 0
        data_count = 0;    //dataread state time counter 0
        idle_count = 0;    //start low voltage level time counter 0
    end

assign oDATA_REAY = DATA_REAY;

always @(negedge iRST_n or  posedge iCLK )
   begin
      if(!iRST_n)
         idle_count <= 0;   //rst
      else
         begin
            if( idle_count_flag )    //the counter start when the  flag is set 1
                idle_count <= idle_count + 1'b1;
            else
                idle_count <= 0;      //the counter stop when the  flag is set 0                        
         end
   end

always @( negedge iRST_n  or  posedge iCLK)
   begin
      if( !iRST_n )
         idle_count_flag <= 1'b0;   // reset off
       else

         begin
               if((state == IDLE) &&(!iIRDA))       // negedge start
                    idle_count_flag <= 1'b1;        //on
                else                                //negedge
                    idle_count_flag <= 1'b0;        //off               
         end
   end

always @(negedge iRST_n or posedge iCLK) begin
    if(!iRST_n)
         state_count <= 0;   //rst
    else
         begin
            if( state_count_flag )
                state_count <= state_count + 1'b1;
            else
                state_count <= 0;
    end
end

always @( negedge iRST_n or posedge iCLK) begin
    if (!iRST_n)
        state_count_flag <= 1'b0;
    else
            begin
                if((state == GUIDANCE) &&(iIRDA))
                    state_count_flag <= 1'b1;
                else
                    state_count_flag <= 1'b0;
    end
end

always @(negedge iRST_n or posedge iCLK) begin
    if(!iRST_n)
        state <= IDLE;
    else

          begin
                if ((state == IDLE) &&(idle_count > GUIDE_LOW_DUR))
                   state <= GUIDANCE;
                else if ( state == GUIDANCE )
                       begin
                         if( state_count > GUIDE_HIGH_DUR )
                             state <= DATAREAD;
                       end
                     else if(state == DATAREAD)
                            begin
                                if( (data_count >= IDLE_HIGH_DUR ) || (bitcount>= 6'b100001) )
                                     state <= IDLE;
                            end
                           else
                              state <= IDLE;
          end
end

always @(negedge iRST_n or  posedge iCLK)
   begin
      if(!iRST_n)
         data_count <= 1'b0;
      else

         begin
            if(data_count_flag)
                data_count <= data_count + 1'b1;
            else
                data_count <= 1'b0;
         end
   end

always @(negedge iRST_n  or posedge iCLK  )
    begin
       if(!iRST_n)
            data_count_flag <= 0;
       else

            begin
             if(state == DATAREAD)
               begin
                 if(iIRDA)
                     data_count_flag <= 1'b1;
                 else
                     data_count_flag <= 1'b0;
               end
             else
               data_count_flag <= 1'b0;
         end
    end

always @(negedge iRST_n or posedge iCLK )
    begin
        if( (!iRST_n) )
            DATA <= 0;      //rst
        else

            begin
             if (state == DATAREAD )
              begin
                 if(data_count >= DATA_HIGH_DUR )
                    DATA[bitcount-1'b1] <= 1'b1;
                 else
                    begin
                    if(DATA[bitcount-1'b1]==1)
                        DATA[bitcount-1'b1] <= DATA[bitcount-1'b1];
                    else
                        DATA[bitcount-1'b1] <= 1'b0;
                    end
              end
             else
                 DATA <= 0;
           end
    end

always @(negedge iRST_n or posedge iCLK )
begin
    if (!iRST_n)
      bitcount <= 1'b0;
    else

      begin
        if(state == DATAREAD)
            begin
                if( data_count == 20000 )
                    bitcount <= bitcount + 1'b1;
            end
        else
           bitcount <= 1'b0;
      end
end

always @(negedge iRST_n or posedge iCLK)
begin
    if(!iRST_n)
       DATA_REAY <=1'b0;
    else

       begin
        if(bitcount == 6'b100000)
            begin
                if( DATA[31:24]== ~DATA[23:16])
                  begin
                    DATA_BUF <= DATA;
                    DATA_REAY <= 1'b1;
                  end
                else
                    DATA_REAY <=1'b0;
            end
        else
            DATA_REAY <=1'b0;
       end
end
//read data
always @(negedge iRST_n or posedge iCLK)
begin
    if(!iRST_n)
        oDATA <= 32'b0000;
     else
     begin
        if(iREAD && DATA_REAY)
           oDATA <= DATA_BUF;
     end
end

endmodule

