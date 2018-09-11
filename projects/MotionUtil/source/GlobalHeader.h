/**
 *  @file   GlobalHeader.h
 *  @brief  共通して使用する変数など.
 */

#ifndef _GLOBALHEADER_H
#define _GLOBALHEADER_H

#include "sxsdk.cxx"

/**
 * プラグインインターフェイス派生クラスのID.
 */
// CMorphWindowInterfaceクラス.
#define MORPH_WINDOW_INTERFACE_ID sx::uuid_class("5A8A6739-E978-4472-B931-7B5A0E331FD5")

// CMotionWindowInterfaceクラス.
#define MOTION_WINDOW_INTERFACE_ID sx::uuid_class("60B54E3B-93DF-4E8A-BF95-46EE7AEEBCF3")

// CRenameDialogクラス.
#define RENAME_DIALOG_ATTRIBUTE_INTERFACE_ID sx::uuid_class("2394DC4D-6FFE-4900-AD2C-9463075F22DC")

/**
 * streamに保存する情報.
 */
// Morph Targets 情報.
#define MORPH_TARGETS_STREAM_ID sx::uuid_class("53DEDAFF-6CE4-4D66-8A3F-D046C5246F9C")

/**
 * streamに保存するバージョン.
 */
#define MORPH_TARGETS_STREAM_VERSION 0x100		// Morph Targets情報保存用.


#endif
