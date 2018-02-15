

acPostProcForPut{
    tudReplicate();
}


tudReplicate{
    *home="/$rodsZoneClient/home/$userNameClient"
    writeLine("serverLog", "Starting tudReplicate on object: *home");

    *Resource="replResc1"

    if($dataSize > 40){
        writeLine("serverLog", "Use slow replication resource");
       *Resource="replResc2"
    }
    else { 
        writeLine("serverLog", "Use fast replication resource"); 
    }

    *err = errormsg( msiDataObjRepl($objPath,"destRescName=*Resource",*Status), *msg );
    if( 0 != *err ) {
        writeLine( "serverLog", "*err - *msg" );
    }
    else {
        writeLine("serverLog", "replication done");
    }
}
