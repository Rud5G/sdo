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

#pragma warning(disable: 4786)
#include "commonj/sdo/HelperProvider.h"
#include "commonj/sdo/XSDHelperImpl.h"
#include "commonj/sdo/XMLHelperImpl.h"
#include "commonj/sdo/export.h"
#include "commonj/sdo/DataFactory.h"

namespace commonj
{
	namespace sdo
	{
		
		//////////////////////////////////////////////////////////////////////
		// Create an XSDHelper
		//////////////////////////////////////////////////////////////////////
		XSDHelperPtr HelperProvider::getXSDHelper(DataFactoryPtr dataFactory)
		{
			return new XSDHelperImpl(dataFactory);
		}
			//////////////////////////////////////////////////////////////////////
		// Create an XMLHelper
		//////////////////////////////////////////////////////////////////////
		XMLHelperPtr HelperProvider::getXMLHelper(DataFactoryPtr dataFactory)
		{
			return new XMLHelperImpl(dataFactory);
		}
		
	} // End - namespace sdo
} // End - namespace commonj