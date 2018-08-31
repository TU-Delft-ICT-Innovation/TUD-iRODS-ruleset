# TUD-iRODS-ruleset
Generic policies and rules for the TU Delft implementation of iRODS


# Update create metadatafile for metadata in collection

The two rules tudUpdateMetadatafileCollection... were created by Arthur Newton of SURFsara under an MIT license. They can be used to create and update *metadata.json* that holds the metadata of the collection in JSON format.

## Dependencies
- Inheritance: for now it is assumed that Inheritance is enabled for every collection -->setInheritOn.re
- iRODS version: 4.1.x and 4.2.x
- Delayed rule needs to be triggered


## FlagMetadatafile
This part, initiated by an acPostProcForModifyAVUMetadata, flags (after possible creation) the metadatafile, metadata.json with attribute "*irods::meta:coll*" and value "*modifed *TimeStamp*".

## tud_delayUpdateMetadatafile 
The second part, a delayed rule to scan for *metadata.json* files that have been modified (via the metadata flag), and subsequently update *metadata.json*. Note that the delayed options for frequency are set to very short times for demo purposes.

## Installation
The file *tudUpdateMetadatafileCollection.re*  needs to be installed in *server_config.json* as a ruleset above *core.re*. 
In order to exercise the rule, the rodsadmin *rods* has to execute the delayed rule *tud_delayUpdateMetadatafile* with *irule tud_delayUpdateMetadatafile "null" "ruleExecOut"*.
