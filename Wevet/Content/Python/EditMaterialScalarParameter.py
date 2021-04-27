# -*- coding: utf-8 -*-

#=======================================================================================================#
#ContentBrowser�őI�����Ă���MaterialInstance�́uParameter.py�v�Ŏw�肵��ScalarParameter�̒l���㏑������#
#=======================================================================================================#
import unreal

#---------------------------#
#GlobalEditorUtility�̃N���X#
#---------------------------#
@unreal.uclass()
class GEditUtil(unreal.GlobalEditorUtilityBase):
	pass

@unreal.uclass()
class MatEditLib(unreal.MaterialEditingLibrary):
	pass


#------------------------------------------------------------------------------#
#Dict�ō��ꂽ�p�����[�^�[�̈ꗗ����������ScalarParameterValue�^�̔z����쐬#
#------------------------------------------------------------------------------#

def SetScalarParameter(PND, MI):
	MEL = MatEditLib()
	PNDKeys = PND.keys()

	for Key in PNDKeys:
		#�w�肵��ScalarParameter�̒l���w�肵���l�ɂ���
		RetVal = MEL.set_material_instance_scalar_parameter_value(MI, unreal.Name(Key),PND[Key])

		#RetVal��False�Ȃ�ҏW�ł��Ȃ������Ƃ������O�͂��Ă邯�ǁA���ۂ͐��False�Ȃ̂ŁA����s�v
'''
		if RetVal == False:
			print("Failed 'set_material_instance_scalar_parameter_value' MI = '"  + MI.get_name() + "' ParamName = '" + Key + "'")
'''

#---------------------------#
#ScalarParameter�̒l���㏑��#
#---------------------------#

def SetSelectedMIScalarParameter(PND):

	Util = GEditUtil()
	AssetList = Util.get_selected_asset()

	#�I�����Ă���A�Z�b�g��For���ŉ񂵂āAMaterialInstance��������A�w�肵��ScalarParameter���w�肵���l�ɂ���
	for Asset in AssetList:
		AssetName = Asset.get_name()

		if type(Asset) != unreal.MaterialInstanceConstant:
			print ("Error:Selected [" + AssetName + "] is Not MaterialInstanceAsset.")
			continue

		MI = unreal.MaterialInstanceConstant.cast(Asset)

		SetScalarParameter(PND, MI)

		print("Edit MaterialParameterValue that " + AssetName + "has.")


