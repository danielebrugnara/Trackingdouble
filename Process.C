#include "Process.h"

//Initialization procedures////////////////////////////////////////////////////////////////////////////////
Process::Process(){
    //Compute front faces here!!
    
}

//Loading event from main//////////////////////////////////////////////////////////////////////////////////
void Process::LoadEvent(const Event & ev){
    this->ev=ev;
    MergePoints();
    npoints=ev.NumberofInteractionPts();
    pth.SetEvent(ev);
    //  SmearPoints();

}

//Closing procedures///////////////////////////////////////////////////////////////////////////////////////

void Process::ClearAll(){
    pth.Clear();
//    interactionorder.clear();
//    distancematr.clear();
    totalfactors.clear();
}

//Merging close points/////////////////////////////////////////////////////////////////////////////////////

void Process::MergePoints(){
    std::vector<int> number_events;
    for (unsigned int i=0; i<npoints; i++){
        number_events.push_back(i);
        for (unsigned int j=i+1; j<npoints;j++){
            if (ev.PointsDistance(i, j)<4){
                number_events.push_back(j);
            }
        }
        if(number_events.size()>1){
            ev.MergePoints(number_events);
        }
        number_events.clear();
    }
}



//Scattering angles/////////////////////////////////////////////////////////////////////////////////////////////////////////
double Process::ComputeScatteringCosAngle(const int & j, const int & k){//maybe matrix?
    Vec3 A=ev.GetInteractionPt(j).GetPosition();
    return A.CosAngleBetween(ev.GetInteractionPt(k).GetPosition()-A);
}

double Process::ComputeScatteringCosAngle(const int & i, const int & j, const int & k){//maype cubic matrix?
    Vec3 A=ev.GetInteractionPt(j).GetPosition();
    return (A-ev.GetInteractionPt(i).GetPosition()).CosAngleBetween((ev.GetInteractionPt(k).GetPosition()-A));
}


//Compton kinematic factor///////////////////////////////////////////////////////////////////////////////////////
double Process::ComputeComptonFactor(const int & i, const int & j, const int & k, const double & E1,  const double & E2){
    double cosang=ComputeScatteringCosAngle(i, j, k);
    double Egeo=E1/(1.0+E1/MEC2*(1.0-cosang));//sistemare
    double err2=SQ(SIGMA_E)*SQ(Egeo/E1*(1+Egeo*(1-cosang)/MEC2))+SQ(SIGMA_R)*SQ(SQ(E1)/(MEC2+SQ(E1)*(1-cosang))*1/(pth.distancematr[i][j]*pth.distancematr[j][k])); //rough approximation, not even sure acout d(cos)/dr
    return exp(-SQ(E2-Egeo)/err2);
}

double Process::ComputeComptonFactor(const int & i, const int & j, const double & E1,  const double & E2){
    double cosang=ComputeScatteringCosAngle(i, j);
    double Egeo=E1/(1.0+E1/MEC2*(1.0-cosang));//sistemare
//    double err2=SQ(sigma_E)*SQ(Egeo/E1*(1+Egeo*(1-cosang)/mec2))+SQ(sigma_r)*SQ(SQ(E1)/(mec2+SQ(E1)*(1-cosang))*1/(distancematr[i][i]*distancematr[i][j])); //rough approximation, not even sure acout d(cos)/dr
    double err2=2;
    double answer=exp(-SQ(E2-Egeo)/err2);
    return answer;
}


//Compton cross section/////////////////////////////////////////////////////////////////////////////////////////
double Process::ComputeNishinaSigma(const int & i, const int & j, const int & k, const double & E1,  const double & E2){
    return 0.5*SQ(RO)*SQ(E2/E1)*(E2/E1+E1/E2-1+SQ(ComputeScatteringCosAngle(i, j, k))); //sistemare
}

double Process::ComputeNishinaSigma(const int & i, const int & j, const double & E1,  const double & E2){
    return 0.5*SQ(RO)*SQ(E2/E1)*(E2/E1+E1/E2-1+SQ(ComputeScatteringCosAngle(i, j))); //sistemare
}

//Total compton cross section///////////////////////////////////////////////////////////////////////////////////////////
double Process::ComputeNishinaSigmaTotal(const double & E){
    //http://lappweb.in2p3.fr/~maire/tutorials/compton.pdf
    double e=E/MEC2;
    return 2.0*PI*SQ(RO)*Z_GE*(((SQ(e)-2*e-2)/(2*CB(e)))*log(2*e+1)+(CB(e)+9*SQ(e)+8*e+2)/(4*SQ(e)*SQ(e)+4*CB(e)+SQ(e)));
}




//Compute the probabilistic factor of a path//////////////////////////////////////////////////////////////////
//double Process::ComputeTotalFactor(const std::vector <int> & interactionorder, std::vector <meritfactor> & meritfactors, const double & etot){
double Process::ComputeTotalFactor(const std::vector <int> interactionorder, std::vector <meritfactor> meritfactors, const double etot){  //passato per value per poterlo vedere nel debugger, cambiare poi
    //////////////////////////////    //////////////////////////////    //////////////////////////////

    
    
    //////////////////////////////    //////////////////////////////    //////////////////////////////


    long double ptmp=-1;
    long double ptmp2=-1;

    size_t size=interactionorder.size();
    double sigma=-1;
    unsigned int i=1;
    
    if (size==1){//maybe not needed, if loop closes right IT IS A MESS::::::CHECK EVERTHING IN THIS METHOD
        sigma=pth.PhotoSigma(0);
        meritfactors[0].nr=-1;
        return NRHA*sigma*exp(-NrhA*sigma*pth.gedistancematr[interactionorder[0]][interactionorder[0]]);
    }else{
        while (meritfactors[i-1].nr==interactionorder[i-1]){
            i++;
        }
        for(; i<size; i++){
            if (i==1){
                sigma=ComputeNishinaSigmaTotal(**);
                ptmp=/*NrhA*/exp(-sigma*NRHA*pth.gedistancematr[interactionorder[i-1]][interactionorder[i-1]]);
//                ptmp*=*ComputeNishinaSigma(interactionorder[i-1], interactionorder[i], E1, E2);
                ptmp2=ComputeComptonFactor(interactionorder[i-1], interactionorder[i], E1, E2);
                meritfactors[i-1].factor=ptmp;
                meritfactors[i-1].nr=interactionorder[i-1];
            }else{
                E1=E2;
                E2=E1-ev.GetInteractionPt(interactionorder[i-1]).GetEnergy();
                ptmp=meritfactors[i-2].factor;
                sigma=ComputeNishinaSigmaTotal(E1);
                ptmp*=ComputeComptonFactor(interactionorder[i-2], interactionorder[i-1], interactionorder[i], E1, E2);
//                ptmp*=ComputeNishinaSigma(interactionorder[i-2], interactionorder[i-1], interactionorder[i], E1, E2);
                ptmp*=/*NrhA*/exp(-NrhA*sigma*pth.gedistancematr[interactionorder[i-2]][interactionorder[i-1]]);
                meritfactors[i-1].nr=interactionorder[i-1];
                meritfactors[i-1].factor=ptmp;

            }
            
        }
//        sigma=photosigma[interactionorder[i-1]];
        sigma=pth.PhotoSigma(i-1);

        meritfactors[i-1].factor=meritfactors[i-2].factor*sigma*NrhA*exp(-NrhA*sigma*pth.gedistancematr[interactionorder[i-2]][interactionorder[i-1]]);
        meritfactors[i-1].nr=interactionorder[i-1];
        return meritfactors[i-1].factor;
        /*
        
        for (i--;i<size-1; i++){
            if (i<1){
                E1=etot;
                E2=E1-ev.GetInteractionPt(interactionorder[0]).GetEnergy();
                sigma=ComputeNishinaSigmaTotal(E1);
                meritfactors[0].factor=exp(-sigma*NrhA*gedistancematr[interactionorder[0]][interactionorder[0]]);
                meritfactors[0].nr=interactionorder[0];
            }else{
                ptmp=meritfactors[i-1].factor;
                E1=E2;
                E2=E1-ev.GetInteractionPt(interactionorder[i]).GetEnergy();
                ptmp*=ComputeComptonFactor(interactionorder[i-1], interactionorder[i], interactionorder[i+1], E1, E2);
                ptmp*=ComputeNishinaSigma(interactionorder[i-1], interactionorder[i], interactionorder[i+1], E1, E2);
                ptmp*=NrhA*exp(-NrhA*sigma*gedistancematr[interactionorder[i-1]][interactionorder[i]]);
                meritfactors[i].nr=interactionorder[i];
                meritfactors[i].factor=ptmp;
            }
        }
        meritfactors[i].factor=meritfactors[i-1].factor*gedistancematr[interactionorder[i-1]][interactionorder[i]]*photosigma[interactionorder[i]];
        meritfactors[i].nr=interactionorder[i];
        return meritfactors[i].factor;
         */
    }
}

//Computes all permutations of the fragmented event//////////////////////////////////////////////////////////////
finalevent Process::ComputeDoubleProbability(){
    std::vector <int> interactionorder;
    double p1=0, p2=0;
    double ptotal=0, ptmp=0;
    double etot1=0, etot2=0;
    finalevent final;
    std::vector<meritfactor> meritfactors1;
    std::vector<meritfactor> meritfactors2;
    interactionorder.resize(npoints);
    for (unsigned int i=0; i<npoints; i++){
        interactionorder[i]=i;
    }
    
    
    for(unsigned int it=1; it<interactionorder.size(); it++){
        std::vector<int> part1(&interactionorder.front(), &interactionorder[it]);
        std::vector<int> part2(&interactionorder[it], &interactionorder[interactionorder.size()]);
        meritfactors1.resize(part1.size());
        meritfactors2.resize(part2.size());
        
        for(unsigned int i=0; i<it;i++){//controllare siano giusti it qui!!
            meritfactors1[i].nr=-1;
            etot1+=ev.GetInteractionPt(i).GetEnergy();
            
        }
        for(unsigned int i=0; i<meritfactors2.size();i++){//e qui
            meritfactors2[i].nr=-1;
            etot2+=ev.GetInteractionPt(i+it).GetEnergy();//e qui
        }
        
        do{
            p1=ComputeTotalFactor(part1, meritfactors1, etot1);
            do{
                p2=ComputeTotalFactor(part2, meritfactors2, etot2);
                ptmp=p1*p2;
                if (ptmp>ptotal){
                    ptotal=ptmp;
                    final.factor=ptotal;
                    final.order1=part1;
                    final.order2=part2;
                }
            }while (std::next_permutation(part2.begin(), part2.end()));
        }while (std::next_permutation(part1.begin(), part1.end()));
    }
    return final;
}

//Computes all permutations of the entire event////////////////////////////////////////////////////////////////
finalevent Process::ComputeSingleProbability(){
    std::vector <int> interactionorder;
    double p=0;
    double pfinal=0;
    double etot=ev.GetTotalEnergy();
    finalevent final;
    final.order2.push_back(0);
    std::vector<meritfactor> meritfactors;
    meritfactors.resize(npoints);
    interactionorder.resize(npoints);

    for (unsigned int i=0; i<npoints; i++){
        meritfactors[i].nr=-1;
        interactionorder[i]=i;
    }
    do{
        pth.OrderPoints(interactionorder, etot);
        p=ComputeTotalFactor(interactionorder, meritfactors, etot);
        if (p>pfinal){
            pfinal=p;
            final.factor=pfinal;
            final.order1=interactionorder;
        }
    }while (std::next_permutation(interactionorder.begin(), interactionorder.end()));
    return final;
}


//Evaluating the event//////////////////////////////////////////////////////////////////////////////////////
void Process::EvaluateEvent(std::ostream & out){
    std::cout <<"Calcolo singola" <<std::endl;
    finalevent psingle=ComputeSingleProbability();
    std::cout <<"Calcolo doppia" <<std::endl;
    
    finalevent pdouble=ComputeDoubleProbability();
    double factor= (pdouble.factor-psingle.factor)/psingle.factor;
    if (factor>treashold||1){
        Print(out, pdouble.order1[0], pdouble.order2[0]);
        
    }
}
 //Print to file the result///////////////////////////////////////////////////////////////////////////////
void Process::Print(std::ostream & out, const int & a, const int & b){
    out <<"$"<<std::endl;
    out <<ev.GetInteractionPt(a)<<std::endl;
    out <<ev.GetInteractionPt(b)<<std::endl;
}
