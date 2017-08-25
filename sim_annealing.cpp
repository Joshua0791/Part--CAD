#include <map>
#include <math.h>
#include <algorithm>
#include <array>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <time.h>
using namespace std;

class node {                //Class for gates
    public:
        string name, outname; //name indicates cell type (nand, nor etc.), outname denotes the output wire name
        vector<string> input,out,in; //input names,primary outputs, primary inputs
        long double Cload; //load cap of this node
        vector<node*> inputs; //fanin nodes of this node
        vector<node*> outputs; //fanout nodes of this node
        vector<node*> outputs_a; //fanout nodes of this node
        int outputs_asize;
        vector<node*> outputs_b; //fanout nodes of this node
        int outputs_bsize;
        int num_in;         //number of inputs
        int processed_in;  // number of inputs processed
        int flag_out;       //flag to check of its a primary output
        int flag;  //processed or not
        int prim_inp,hyp_flag,parta;
        void outputs_alloc(int n) { outputs.resize(n); }    //resize vectors
        void alloc(int n) { input.resize(n); }  //resize vectors
        void inputs_alloc(int n) { inputs.resize(n); }  //resize vectors
};
vector<node*> parta;
vector<node*> partb;
vector<node*> part;

int c,cmin,accepted,lucky,tot_size;
float multi;
int partasize,partbsize;

vector<node*> netlist(ifstream &fp,ofstream &op2)
{
    vector<node*> gates;
    vector<node*> gates_1;
    vector<node*> que;
    vector<string> out,in;
    node *p1,*p2;
    int gate_cnt=0,gate_cnt_1=0,out_cnt=0,in_cnt=0,nand_cnt=0,nor_cnt=0,or_cnt=0,and_cnt=0,not_cnt=0,xor_cnt=0,buff_cnt=0;
    string s,stemp;
    if(fp.is_open()) {
        while(getline(fp, s)){
            if(s.find("=") < s.length()){
                p1= new node ;
                (*p1).num_in=0;
                p1->flag_out=0;
                p1->prim_inp=0;
                p1->hyp_flag=0;
                p1->outputs_asize=0;
                p1->outputs_bsize=0;
                p1->outname=s.substr(0, s.find("=")-1);
                (*p1).name=s.substr(s.find("=")+2,(s.find("(")-s.find("=")-2));
                stemp=p1->name;
                if(stemp.compare("NAND")==0){ nand_cnt++;}
                if(stemp.compare("NOR")==0){ nor_cnt++;}
                if(stemp.compare("OR")==0){ or_cnt++;}
                if(stemp.compare("AND")==0){ and_cnt++;}
                if(stemp.compare("BUFF")==0) { buff_cnt++;}
                if(stemp.compare("XOR")==0) { xor_cnt++;}
                if(stemp.compare("NOT")==0) { not_cnt++;}
                if(stemp.compare("NOT")==0 || stemp.compare("BUFF")==0) {
                            p1 -> alloc((*p1).num_in+1);
                            (*p1).input[(*p1).num_in]=(s.substr(s.find("(")+1, (s.find(")") - s.find("("))-1));
                            (*p1).num_in++;
                            gate_cnt++;
                            gates.resize(gate_cnt);
                            gates[gate_cnt-1]=p1;
                            gate_cnt_1++;
                            gates_1.resize(gate_cnt_1);
                            gates_1[gate_cnt_1-1]=p1;

                }
                else {
                p1 -> alloc((*p1).num_in+1);
                (*p1).input[(*p1).num_in]=(s.substr(s.find("(")+1, (s.find(",") - s.find("("))-1));
                stemp = s.substr(s.find(",")+2,(s.find(")") - s.find(","))-2);
                (*p1).num_in++;
                while(stemp.find(",") < stemp.length()) {
                    p1 -> alloc((*p1).num_in+1);
                    (*p1).input[(*p1).num_in]=(stemp.substr(0,stemp.find(",")));
                    (*p1).num_in++;
                    stemp = stemp.substr(stemp.find(",")+2);
                }
                p1 -> alloc((*p1).num_in+1);
                (*p1).input[(*p1).num_in]=(stemp);
                (*p1).num_in++;
                gate_cnt++;
                gate_cnt_1++;
                gates_1.resize(gate_cnt_1);
                gates_1[gate_cnt_1-1]=p1;
                gates.resize(gate_cnt);
                gates[gate_cnt-1]=p1;
                }
            }
            if(s.find("OUTPUT") < s.length()){
                out.resize(out_cnt+1);
                out[out_cnt]=s.substr(s.find("(")+1, (s.find(")") - s.find("("))-1);
                out_cnt++;
            }
            if(s.find("INPUT") < s.length()){
                p2=new node ;
                 (*p2).num_in=0;
                p2->flag_out=0;
                //p2->prim_inp=0;
                p2->hyp_flag=0;
                gate_cnt_1++;
                gates_1.resize(gate_cnt_1);
                gates_1[gate_cnt_1-1]=p2;
                in.resize(in_cnt+1);
                in[in_cnt]=s.substr(s.find("(")+1, (s.find(")") - s.find("("))-1);
                p2->outname=in[in_cnt];
                p2->prim_inp=1;
                p2->name="INPUT";
                in_cnt++;
            }

        }
}
    //cout << "hi";
    op2 << in_cnt << " primary inputs\n";
    op2 << out_cnt << " primary outputs\n";
    op2 << nand_cnt << " NAND gates\n";
    op2 << nor_cnt << " NOR gates\n";
    op2 << or_cnt << " OR gates\n";
    op2 << and_cnt << " AND gates\n";
    op2 << not_cnt << " NOT gates\n";
    op2 << buff_cnt << " BUFF gates\n";
    op2 << xor_cnt << " XOR gates\n";
    op2 << gate_cnt << " Total gates\n";
    op2 << "\nFANOUT \n";
    int i=0,j=0,l=0,k=0;

    while(i<gates_1.size()){
        l=0;
        j=0;
        node *a1=gates_1[i];
        //while(a1->prim_inp==1){i++;a1=gates_1[i];}
        string s1=a1->outname;
        op2 << a1->name << "-" << s1 <<":";
        while(j<gates.size()){
               // cout << "hello";
            node *a2=gates[j];
            for(k=0;k<(*a2).num_in;k++){
                string s2=a2->input[k];
                if((s1.compare(s2))==0) {
                    a1->outputs.push_back(a2);
                    op2<< " " << a2->name<<"-" << (*a2).outname ;
                    l++;
                }
            }
            j++;
        }
        j=0;
        while(j<out_cnt){
            if((s1.compare(out[j])==0)) {
                a1->flag_out++;
                a1->out.push_back(out[j]);
                op2 << " OUTP";
            }
            j++;
        }
        i++;
        op2 << "\n" ;
    }
    //cout << "hi";
    op2 << "\nFANIN \n";
    i=0;j=0;l=0;k=0;
    int flag=0;
    while(i<gates_1.size()){
        l=0;
        j=0;
        string s1;
        node *a1=gates_1[i];
        while(a1->prim_inp==1){i++;a1=gates_1[i];}
        a1->flag=0;
        a1->processed_in=0;
        //a1->rat=0;
        flag=0;
        op2 << a1->name << "-" << a1->outname <<":" ;
        for(k=0;k<(*a1).num_in;k++){
            s1=a1->input[k];
            j=0;
            while(j<gates_1.size()){
                node *a2=gates_1[j];
                string s2=a2->outname;
                if((s1.compare(s2))==0) {
                    a1->inputs.push_back(a2);
                    op2<< " " << a2->name<<"-" << a2->outname << "" ;
                    l++;
                }
                j++;
            }
            j=0;
            while(j<in_cnt){
                if((s1.compare(in[j])==0)) {
                    flag++;
                    a1->processed_in++;
                    //a1->inputs.push_back(a2);
                    a1->in.push_back(in[j]);
                    if( flag == a1->num_in ) {
                    que.push_back(a1);}
                    op2 << " INP-" << s1;
                }
                j++;
            }
        }
        i++;
        op2 << "\n";
    }
op2.close();
return gates_1;
}

int find_size(vector<node*> arr)
{
    int part_size=0,j=0;
    node* p1;
    while( j < arr.size()  ){
        p1=arr[j];
        if(p1->prim_inp==1){part_size+=1;}
        else{part_size+=p1->num_in;}
        j++;
    }
    return part_size;
}

int find_cost(vector<node*> Newpart_a, vector<node*> Newpart_b)
{
   vector<node*> newpart_a; vector<node*> newpart_b;
    newpart_a=Newpart_a;
    newpart_b=Newpart_b;
   node* p1;
   node* p2;
   int cut_cost=0,i=0,j=0,k=0;
   while(i<newpart_a.size())
   {
       p1=newpart_a[i];
       p1->hyp_flag=0;
       for(k=0;k<(p1->outputs.size());k++){
            j=0;
       while( j<newpart_b.size() && p1->hyp_flag==0 )
       {
           p2=newpart_b[j];
           if((p1->outputs[k])->outname==p2->outname){cut_cost++;p1->hyp_flag=1;}
           j++;
       }
       }
       i++;
   }
   i=0;
   while(i<newpart_b.size())
   {
       p1=newpart_b[i];
       p1->hyp_flag=0;
       j=0;
       for(k=0;k<(p1->outputs.size());k++){
            j=0;
       while(j<newpart_a.size() && p1->hyp_flag==0)
       {
           p2=newpart_a[j];
           if((p1->outputs[k])->outname==p2->outname){cut_cost++;p1->hyp_flag=1;}
           j++;
       }

       }
       i++;
   }
   return cut_cost;
}

double find_delta_cost_change(node* p1, int option)
{
   node* p2;
   int flag_a=0,flag_b=0,i=0,j=0,k=0;
   double cut_cost_change=0;
  if(option==1){
    if(p1->outputs_asize>0){
        cut_cost_change=1;
        if(p1->outputs_bsize>0){
        cut_cost_change=cut_cost_change-1;
        }
    }
    else { if(p1->outputs_bsize>0){
        cut_cost_change=-1;
        }
    }
    j=0;
    while(j<p1->inputs.size())
    {
        node* p2= p1->inputs[j];
        if(p2->parta==1 && p2->outputs_asize>=1 && p2->outputs_bsize==0){cut_cost_change++;}// cout<< "Found input in part A with no outputs in part B +1" <<endl;}
        if(p2->parta==0 && p2->outputs_asize==1){cut_cost_change--;}// cout<< "Found input in part B with no other outputs in part B -1" << endl;}
    j++;
  }
  }
    if(option==0){
        if(p1->outputs_bsize>0){
            cut_cost_change=1;
            if(p1->outputs_asize>0){
                    cut_cost_change=cut_cost_change-1;
            }
        }
        else { if(p1->outputs_asize>0){
            cut_cost_change=-1;
            }
        }
        j=0;
        if(p1->prim_inp==0){
    while(j<p1->inputs.size())
    {
        node* p2= p1->inputs[j];
        if(p2->parta==0 && p2->outputs_bsize>=1 && p2->outputs_asize==0){cut_cost_change=cut_cost_change+1;}//cout<< "Found input in part B with no outputs in part A +1" <<endl;}
        if(p2->parta==1 && p2->outputs_bsize==1){cut_cost_change=cut_cost_change-1;}//cout<< "Found input in part A with no other outputs in part A -1" << endl;}
     j++;
  }
}
}
   return cut_cost_change;
}

void new_solution_change(node* p1,int option)
{
    int j=0,i=0,k=0;
    if(option==1){p1->parta=0;}
    if(option==0){p1->parta=1;}
    while(j<p1->inputs.size())
    {
        node* p2= p1->inputs[j];
        if(option==1){
                p2->outputs_asize--;
                p2->outputs_bsize++;
        }
        if(option==0){
                p2->outputs_asize++;
                p2->outputs_bsize--;
        }
        j++;
    }

}

void perturb_que(double T)
{
    int random=0,asize=0,bsize=0,cNew=0,i=0,r;node* temp;
    int option;
    const double e = 2.71;
    bsize=partbsize;
    asize=partasize;
    random = rand() % part.size();
    temp=part[random];
    if(part[random]->parta==1){
            option=1;
        bsize=partbsize+temp->num_in;
        asize=partasize-temp->num_in;
        if(temp->prim_inp==1){bsize=partbsize+1;asize=partasize-1;}
    }
    if(part[random]->parta==0){
        option=0;
        asize=partasize+temp->num_in;
        bsize=partbsize-temp->num_in;
        if(temp->prim_inp==1){asize=partasize+1;bsize=partbsize-1;}
    }
    cNew = c + find_delta_cost_change(temp,option);
    if (cNew < c || (rand() / (double)RAND_MAX) <= pow(e, -(cNew - c) / (T)))
            {
                if(cNew<cmin){cmin=cNew;}
                if(cNew>c){lucky++;}
                if((asize<=bsize*multi && asize>=bsize)||(bsize<=asize*multi && bsize>=asize)){
                    accepted++;
                    partasize=asize;
                    partbsize=bsize;
                    c = cNew;
                    new_solution_change(temp,option);
                }else{
                     r=rand()%2+1;
            switch(r){
            case 1: perturb_que(T); break;//}
            case 2: perturb_que(T); break;//}
            }}
            }
}
void perturb_swap_que(double T)
{
    int random=0,asize=0,bsize=0,cNew=0,i=0;node* temp;node* temp1;
    const double e = 2.718281828;
    bsize=partbsize;
    asize=partasize;
    random = rand() % part.size();
    temp= part[random];
    while(temp->parta==0){
        random = rand() % part.size();
        temp= part[random];}
        bsize=partbsize+temp->num_in;
        asize=partasize-temp->num_in;
        if(temp->prim_inp==1 && temp->parta==1){bsize=partbsize+1;asize=partasize-1;}
       // if(temp->prim_inp==1 && temp->parta==0){bsize=partbsize-1;asize=partasize+1;}
        cNew = c + find_delta_cost_change(temp,1);
        new_solution_change(temp,1);
        //cout << "hi";;
        random = rand() % part.size();
        temp1= part[random];
        while(temp1->parta==1 || temp==temp1){
        random = rand() % part.size();
        temp1= part[random];}
        cNew = cNew + find_delta_cost_change(temp1,0);
        asize=asize+temp1->num_in;
        bsize=bsize-temp1->num_in;
        if(temp1->prim_inp==1 && temp1->parta==0){asize=asize+1;bsize=bsize-1;}
        //if(temp1->prim_inp==1 && temp->parta==1){asize=asize-1;bsize=bsize+1;}
  if (cNew < c || (rand() / (double)RAND_MAX) <= pow(e, -(cNew - c) / (T)))
            {
                if(cNew<cmin){cmin=cNew;}//cout<<"#################################################################################################";}
 if(cNew>c){lucky++;}
                if((asize<=bsize*multi && asize>=bsize)||(bsize<=asize*multi  && bsize>=asize)       ){
                    accepted++;
                    partasize=asize;
                    partbsize=bsize;
                    c = cNew;
                    new_solution_change(temp1,0);
                }else{new_solution_change(temp,0);//cout << endl;
            perturb_swap_que(T);//}
            }//cout <<":Declined ratio" <<endl;}
            }else{new_solution_change(temp,0);}//cout << endl;}//cout <<":Declined" <<endl;}
}
void populate_part_connections(vector<node*> gates_1)
{

    int i=0,j=0,l=0,k=0;

    while(i<gates_1.size()){
        l=0;
        j=0;
        node *a1=gates_1[i];
        string s1=a1->outname;
        while(j<parta.size()){//cout << "hi"<< parta[j]->outname;
            node *a2=parta[j];
            if(a1->prim_inp==1){while(a2->prim_inp==1){j++;a2=parta[j];}}
            for(k=0;k<(*a2).num_in;k++)
                {
                string s2=a2->input[k];
                if((s1.compare(s2))==0) {
                    a1->outputs_asize++;
                    l++;
                }
            }

            j++;//cout << "hey";
        }
        j=0;
        while(j<partb.size()){
            node *a2=partb[j];
            if(a1->prim_inp==1){while(a2->prim_inp==1){j++;a2=partb[j];}}
            for(k=0;k<(*a2).num_in;k++){
                string s2=a2->input[k];
                if((s1.compare(s2))==0) {
                    a1->outputs_bsize++;
                    l++;
                }
            }
            j++;
        }
        i++;
    }
}
void sim_annealing()
{

    int r;
    double  alpha = 0.97  ;                         //alpha is used for the cooling schedule of the temperature

   for (double T = 10; T > 0.8 ; T *= alpha) //T = T * alpha which used as a cooling schedule
    {
        accepted=0;
        lucky=0;
        for (int i = 0; i<3000; i++) //This loop is for the process of iteration (or searching for new states)
        {
            r=rand()%2+1;
            switch(r){
            case 1: perturb_que(T); break;//}
            case 2: perturb_que(T); break;//}
            case 3: perturb_swap_que(T); break;//}
            }
        }//cout << "Accepted " << accepted << " for T= " << T << " Lucky " << lucky << " cost " << c << endl;
}
        alpha=0.999;
        for (double T = 0.8; T > 0.2 ; T *= alpha) //T = T * alpha which used as a cooling schedule
    {
        accepted=0;
        lucky=0;
        for (int i = 0; i<8000; i++) //This loop is for the process of iteration (or searching for new states)
        {

            r=rand()%2+1;
            switch(r){
            case 1: perturb_que(T); break;//}
            case 2: perturb_que(T); break;//}
            case 3: perturb_swap_que(T); break;//}
            }
        }
       // cout << "Accepted " << accepted << " for T= " << T << " Lucky " << lucky << " cost " << c << endl;
    }
        alpha=0.98;
        for (double T =0.2; T > 0.01 ; T *= alpha) //T = T * alpha which used as a cooling schedule
    {   //cout << "hi";
        accepted=0;
        lucky=0;
        for (int i = 0; i<300; i++) //This loop is for the process of iteration (or searching for new states)
        {
            r=rand()%3+1;
            switch(r){
            case 1: perturb_que(T); break;//}
            case 2: perturb_que(T); break;//}
            case 3: perturb_swap_que(T); break;//}
            }
        }//cout << "Accepted " << accepted << " for T= " << T << " Lucky " << lucky << " cost " << c << endl;
    }

}

int main(int argc, char *argv[])
{
    string path1,path2,opath1,opath2;
    node* p1;
    vector<node*> que;
    vector<node*> quea;
    vector<node*> queb;
    srand (time(NULL));
    int j=0,i=0,k=0,l=0,m=0,flag=0,cost=0,parta_size=0,partb_size=0;
    multi=1.1;
    path1=argv[1];
    ifstream InputFile1 (path1.c_str());
    ofstream OutputFile1;
    OutputFile1.open("netlist_op.txt");
    ofstream of2;
    of2.open("output.txt");
    que=netlist(InputFile1,OutputFile1);
    tot_size=0;
    tot_size= find_size(que);
    j=0;
    i=0;
     while( j < tot_size  ){
        p1=que[i];
        if(j<(tot_size/2)){
                quea.push_back(p1);
                p1->parta=1;
                if(p1->prim_inp==1){j+=1;}
                else{j+=p1->num_in;}
                //cout<< p1->outname << " ";
        }
        else {
            queb.push_back(p1);
            p1->parta=0;
            if(p1->prim_inp==1){j+=1;}
            else{j+=p1->num_in;}
        }
        i++;
    }
    if(tot_size<50){multi=1.2;}
    parta=quea;
    partb=queb;
    populate_part_connections(que);
    c=find_cost(parta,partb);
    cmin=c;
  partasize=find_size(parta);
    partbsize=find_size(partb);
    part=que;
   sim_annealing();
    of2 << "Cost" << c <<  endl;
    of2 << "\nPart A size:" << partasize << " Part B size:" << partbsize << endl;
    of2 << "\nPart A:";
    for(i=0;i<parta.size();i++){
        of2 << " " << parta[i]->outname << " ";
    }
    of2 << endl << "\nPart B:";
    for(i=0;i<partb.size();i++){
        of2 << " " << partb[i]->outname << " ";
    }
}
