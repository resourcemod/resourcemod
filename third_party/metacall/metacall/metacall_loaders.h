/*
 *	Loader Library by Parra Studios
 *	A library for loading executable code at run-time into a process.
 *
 *	Copyright (C) 2016 - 2022 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 *
 */

#ifndef METACALL_LOADERS_H
#define METACALL_LOADERS_H 1

#define OPTION_BUILD_LOADERS		1

#ifdef OPTION_BUILD_LOADERS
/* #undef OPTION_BUILD_LOADERS_C */
/* #undef OPTION_BUILD_LOADERS_COB */
/* #undef OPTION_BUILD_LOADERS_CR */
#define OPTION_BUILD_LOADERS_CS	1
/* #undef OPTION_BUILD_LOADERS_DART */
/* #undef OPTION_BUILD_LOADERS_EXT */
/* #undef OPTION_BUILD_LOADERS_FILE */
/* #undef OPTION_BUILD_LOADERS_JAVA */
/* #undef OPTION_BUILD_LOADERS_JSM */
/* #undef OPTION_BUILD_LOADERS_JS */
/* #undef OPTION_BUILD_LOADERS_LUA */
/* #undef OPTION_BUILD_LOADERS_MOCK */
#define OPTION_BUILD_LOADERS_NODE	1
#define OPTION_BUILD_LOADERS_TS	1
#define OPTION_BUILD_LOADERS_PY	1
#define OPTION_BUILD_LOADERS_RB	1
/* #undef OPTION_BUILD_LOADERS_RPC */
/* #undef OPTION_BUILD_LOADERS_WASM */
#endif /* OPTION_BUILD_LOADERS */

#endif /* METACALL_LOADERS_H */
