/*
 *  PropertyCell.h
 *  SniperEditorMacOS
 *
 *  Created by Alexey Prosin on 12/13/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PROPERTY_CELL
#define PROPERTY_CELL

#include "DAVAEngine.h"
#include "UICheckBox.h"

using namespace DAVA;

class PropertyValue;
class UICheckBox;

class PropertyCellData;
class PropertyCellDelegate
{
public:
    virtual void OnPropertyChanged(PropertyCellData *changedProperty) = 0;
};

class PropertyCell : public UIListCell
{
protected:
    
    enum eConst
    {
        CELL_HEIGHT = 15,
    };
    
public:
    
    enum CellType 
    {
        PROP_CELL_TEXT = 0,
        PROP_CELL_DIGITS,
        PROP_CELL_FILEPATH,
        PROP_CELL_BOOL,
        
        PROP_CELL_COUNT
    };
    
    PropertyCell(PropertyCellDelegate *propDelegate, const Rect &rect, PropertyCellData *prop);
    virtual ~PropertyCell();

    virtual void SetData(PropertyCellData *prop);

    static String GetTypeName(int cellType);

    PropertyCellData *property;
    UIStaticText *keyName;
    PropertyCellDelegate *propertyDelegate;
};

class PropertyTextCell : public PropertyCell, public UITextFieldDelegate
{
public:
    PropertyTextCell(PropertyCellDelegate *propDelegate, PropertyCellData *prop, float32 width);
    virtual ~PropertyTextCell();
    
    static float32 GetHeightForWidth(float32 currentWidth);
    virtual void SetData(PropertyCellData *prop);

    virtual void TextFieldShouldReturn(UITextField * textField);
    virtual void TextFieldLostFocus(UITextField * textField);
	virtual bool TextFieldKeyPressed(UITextField * textField, int32 replacementLocation, int32 replacementLength, const WideString & replacementString);
    
    virtual void DidAppear();

    UITextField *editableText;
    UIStaticText *uneditableText;
    UIControl *uneditableTextContainer;
};

class PropertyBoolCell : public PropertyCell, public UICheckBoxDelegate
{
public:
    PropertyBoolCell(PropertyCellDelegate *propDelegate, PropertyCellData *prop, float32 width);
    virtual ~PropertyBoolCell();
    
    static float32 GetHeightForWidth(float32 currentWidth);
    virtual void SetData(PropertyCellData *prop);
    
    virtual void ValueChanged(bool newValue);
    
    UICheckBox *checkBox;
    UIStaticText *falseText;
    UIStaticText *trueText;
    UIControl *textContainer;
};

class PropertyFilepathCell : public PropertyCell, public UIFileSystemDialogDelegate
{
public:
    PropertyFilepathCell(PropertyCellDelegate *propDelegate, PropertyCellData *prop, float32 width);
    virtual ~PropertyFilepathCell();
    
    static float32 GetHeightForWidth(float32 currentWidth);
    virtual void SetData(PropertyCellData *prop);
    
    virtual void OnFileSelected(UIFileSystemDialog *forDialog, const String &pathToFile);
    virtual void OnFileSytemDialogCanceled(UIFileSystemDialog *forDialog);

    void OnButton(BaseObject * object, void * userData, void * callerData);

//    virtual void DidAppear();
    
    UIStaticText *pathText;
    UIControl *pathTextContainer;
    UIButton *browseButton;
    UIFileSystemDialog *dialog;
};



#endif