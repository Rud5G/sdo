/* 
+----------------------------------------------------------------------+
| (c) Copyright IBM Corporation 2005.                                  | 
| All Rights Reserved.                                                 |
+----------------------------------------------------------------------+ 
|                                                                      | 
| Licensed under the Apache License, Version 2.0 (the "License"); you  | 
| may not use this file except in compliance with the License. You may | 
| obtain a copy of the License at                                      | 
|  http://www.apache.org/licenses/LICENSE-2.0                          |
|                                                                      | 
| Unless required by applicable law or agreed to in writing, software  | 
| distributed under the License is distributed on an "AS IS" BASIS,    | 
| WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      | 
| implied. See the License for the specific language governing         | 
| permissions and limitations under the License.                       | 
+----------------------------------------------------------------------+ 
| Author: Pete Robbins                                                 | 
+----------------------------------------------------------------------+ 

*/
/* $Id$ */

#ifndef _TYPEDEFINITION_H_
#define _TYPEDEFINITION_H_

#include "commonj/sdo/PropertyDefinition.h"
#include "list"

namespace commonj
{
	namespace sdo
	{
		
		typedef std::list<PropertyDefinition> XmlDasPropertyDefs;
		
		class TypeDefinition
		{
			
		public:
			TypeDefinition();
			virtual ~TypeDefinition();
		
	
			SDOXMLString uri;
			SDOXMLString name;
			SDOXMLString localname;

			SDOXMLString aliases;

			SDOXMLString parentTypeUri;
			SDOXMLString parentTypeName;

			SDOXMLString IDPropertyName;

			bool dataType;
			XmlDasPropertyDefs properties;

			bool isOpen;
			bool isSequenced;
			bool isAbstract;

			bool isMany;
			int groupElementCount;

			bool isQName;
		};
	} // End - namespace sdo
} // End - namespace commonj


#endif //_TYPEDEFINITION_H_
