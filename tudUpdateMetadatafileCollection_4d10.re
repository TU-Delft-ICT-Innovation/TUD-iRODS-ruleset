acPostProcForModifyAVUMetadata(*option, *itemType, *itemName, *attributeName, *attributeValue, *attributeUnit) {
        #this rule checks if metadata has been added/deleted
        #Only check for collections, not for data objects
        if(*itemType=="-c") then {
                FlagMetadatafile(*itemName);
        }

        msiGoodFailure;
}



acPostProcForModifyAVUMetadata(*option,*itemType,*itemName,*attributeName,*attributeValue,*attributeUnit, *newAttributeName, *newAttributeValue, *newAttributeUnit) {
        #this rule checks if metadata has been modified to a new value
        #Only check for collections, not for data objects
        if(*itemType=="-c") then {
                FlagMetadatafile(*itemName);
        }
        
        msiGoodFailure;
}



acPostProcForModifyAVUMetadata(*Option,*SourceItemType,*TargetItemType,*SourceItemName,*TargetItemName) {
        #this rule checks if metadata has been copied over to the collection
        #Only check for collections, not for data objects
        if(*TargetItemType=="-c") then {
                FlagMetadatafile(*TargetItemName);
        }
        
        msiGoodFailure;
}







FlagMetadatafile(*itemName) {
        #function to flag metadata.json files with "outofdate" to be picked up by tudUpdateMetadatafile
        #check if file metadata.json exists and has not been deleted
        #if file is deleted: create it again and set correct permissions (rodsadmin rods needs to be able to change metdata.json)
        *metadatafileColl = "*itemName/metadata.json";
        *errcode = errorcode(msiObjStat(*metadatafileColl,*Stat));
        if(*errcode<0) {
                *OFlagsB="forceFlag=true"
                msiDataObjCreate(*metadatafileColl,*OFlagsB,*D_FD);
                msiDataObjClose(*D_FD,*Status);

                *Acl = "own"
                *User = "rods"
                msiSetACL("default",*Acl,*User,*metadatafileColl);
        }
            
        #Create Key-Value pair with timestamp
        *KeyColl="irods::meta::coll";
        msiGetSystemTime(*Time,"human");
        *ValColl="outofdate *Time";
        msiAddKeyVal(*Keyval,*KeyColl,*ValColl);
           
        #Add Key-Value pair as metadata to metadata.json file within collection (this file is made upon creation of collection)
        #A delayed rule, tudDelayUpdateMetadatafile.r, will search for files with the attribute "irods::meta::coll" and value "modified%".
        errorcode(msiSetKeyValuePairsToObj(*Keyval,*metadatafileColl,"-d"));

}




tudUpdateMetadatafile {
        #delayed rule to check for all metadata.json files which are flagged with metadata item "outofdate". 
        #Every collection which has metadata should have a metdatafile object inside with its metadata.
        delay("<PLUSET>1s</PLUSET><EF>2s</EF>") {
                #Find collection which metadata has recently been modified
                foreach(*row1 in SELECT COLL_NAME where DATA_NAME like "%metadata.json" and META_DATA_ATTR_NAME like "irods::meta::coll" and META_DATA_ATTR_VALUE like "outofdate%") {
                        *collPath=*row1.COLL_NAME;
                        writeLine("serverLog","FOUND METADATAFILE in COLLECTION *collPath");   

                        #Recreate metadatafile within this collection with forceFlag=true (if file already exists)
                        #Ideally this should be a new microservice to convert iRODS metadata to JSON
                        *metadatafile = "*collPath/metadata.json"
                        *OFlagsB="forceFlag=true"
                        msiDataObjCreate(*metadatafile,*OFlagsB,*D_FD);
                        *lineOfText="{\n";
                        msiStrToBytesBuf(*lineOfText,*R_BUF);
                        msiDataObjWrite(*D_FD,*R_BUF,*W_LEN);

                        #Find all metadata keys and values and write to metadata.json file
                        foreach(*row2 in SELECT META_COLL_ATTR_NAME, META_COLL_ATTR_VALUE where COLL_NAME = *collPath) {
                                *mdcollname = *row2.META_COLL_ATTR_NAME;
                                *mdcollvalue  = *row2.META_COLL_ATTR_VALUE;
                                *lineOfText='"'++*mdcollname++'":"'++*mdcollvalue++'",\n';
                                msiStrToBytesBuf(*lineOfText,*R_BUF);
                                msiDataObjWrite(*D_FD,*R_BUF,*W_LEN);
                        }
                        *lineOfText='"collection name":"'++*collPath++'"\n}\n';
                        msiStrToBytesBuf(*lineOfText,*R_BUF);
                        msiDataObjWrite(*D_FD,*R_BUF,*W_LEN);
                        msiDataObjClose(*D_FD,*Status);

                        #Reset metadata of the metadata.json to not modified.
                        *KeyColl="irods::meta::coll";
                        msiGetSystemTime(*Time,"human");
                        *ValColl="uptodate *Time";
                        msiAddKeyVal(*Keyval,*KeyColl,*ValColl);
                        #*metadatafileColl = "*collPath/metadata.json";
                        errorcode(msiSetKeyValuePairsToObj(*Keyval,*metadatafile,"-d"));
                        
                }
        }
}

