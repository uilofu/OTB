#include "ers/ErsSar/ErsSarLeader/ErsSarLeader.h"
#include "ers/ErsSar/ErsSarLeader/ErsSarLeaderFactory.h"
#include "ers/ErsSar/ErsSarRecordHeader.h"

#include "ers/ErsSar/ErsSarLeader/ErsSarFileDescriptor.h"
#include "ers/ErsSar/ErsSarLeader/ErsSarDataSetSummary.h"
#include "ers/ErsSar/ErsSarLeader/ErsSarMapProjectionData.h"
#include "ers/ErsSar/ErsSarLeader/ErsSarFacilityData.h"

const int ErsSarLeader::ErsSarFacilityDataID = 5;
const int ErsSarLeader::ErsSarPlatformPositionDataID = 4;
const int ErsSarLeader::ErsSarMapProjectionDataID = 3;
const int ErsSarLeader::ErsSarDataSetSummaryID = 2;
const int ErsSarLeader::ErsSarFileDescriptorID = 1;

ErsSarLeader::ErsSarLeader()
{
}

ErsSarLeader::~ErsSarLeader()
{
	ClearRecords();
}

std::ostream& operator<<(std::ostream& os, const ErsSarLeader& data)
{
	std::map<int, ErsSarRecord*>::const_iterator it = data._records.begin();
	while(it != data._records.end())
	{
		(*it).second->Write(os);
		++it;
	}
	return os;

}

std::istream& operator>>(std::istream& is, ErsSarLeader& data)
{
	ErsSarLeaderFactory factory;

	data.ClearRecords();

	ErsSarRecordHeader header;
	bool eof = false;
	while(!eof)
	{
		is>>header;
		if(is.eof())
		{
			eof = true;
		}
		else
		{
			ErsSarRecord* record = factory.Instanciate(header.get_rec_seq());
			if (record != NULL)
			{
				record->Read(is);
				data._records[header.get_rec_seq()] = record;
			}
			else
			{
				char* buff = new char[header.get_length()-12];
				is.read(buff, header.get_length()-12);
				delete buff;
			}
		}
	}
	return is;
}


ErsSarLeader::ErsSarLeader(const ErsSarLeader& rhs)
{
	std::map<int, ErsSarRecord*>::const_iterator it = rhs._records.begin();
	while(it != rhs._records.end())
	{
		_records[(*it).first] = (*it).second->Clone();
		++it;
	}
}

ErsSarLeader& ErsSarLeader::operator=(const ErsSarLeader& rhs)
{
	ClearRecords();
	std::map<int, ErsSarRecord*>::const_iterator it = rhs._records.begin();
	while(it != rhs._records.end())
	{
		_records[(*it).first] = (*it).second->Clone();
		++it;
	}

	return *this;
}

void ErsSarLeader::ClearRecords()
{
	std::map<int, ErsSarRecord*>::const_iterator it = _records.begin();
	while(it != _records.end())
	{
		delete (*it).second;
		++it;
	}
	_records.clear();
}

bool ErsSarLeader::saveState(ossimKeywordlist& kwl,
                             const char* prefix) const
{
   /*
   static const char MODULE[] = "ErsSarModel::saveState";

   if (traceDebug())
   {
      ossimNotify(ossimNotifyLevel_DEBUG)<< MODULE << " entered...\n";
   }
   */
  
  std::cout << Here!! << std::endl;

  bool result = true;

	char name[64];

	kwl.add(prefix, ossimKeywordNames::TYPE_KW, "ossimErsSarModel", true);
	/*
	 * Adding metadata necessary to the sensor model in the keywordlist
	 */
	ErsSarFileDescriptor *leaderfiledesc = get_ErsSarFileDescriptor();
	if (leaderfiledesc != NULL)
	{
		kwl.add(prefix, "filename",leaderfiledesc->get_file_name().c_str(),true);
	}
	else
	{
	  result = false;
  }
	
	/*
	 * Adding metadata necessary to the sensor model in the keywordlist
	 */
	ErsSarDataSetSummary *datasetSummary = get_ErsSarDataSetSummary();
	if ( (datasetSummary != NULL) && (result == true) )
	{
		kwl.add(prefix, "inp_sctim",(datasetSummary->get_inp_sctim()).c_str(),true);
		kwl.add(prefix, "ellip_maj", datasetSummary->get_ellip_maj(),true);
		kwl.add(prefix, "ellip_min", datasetSummary->get_ellip_min(),true);
		kwl.add(prefix, "sc_lin", datasetSummary->get_sc_lin(),true);
		kwl.add(prefix, "sc_pix", datasetSummary->get_sc_pix(),true);
		kwl.add(prefix, "wave_length", datasetSummary->get_wave_length(),true);
		kwl.add(prefix, "fr", datasetSummary->get_fr(),true);
		kwl.add(prefix, "fa", datasetSummary->get_fa(),true);
		kwl.add(prefix, "time_dir_pix", (datasetSummary->get_time_dir_pix()).c_str(),true);
		kwl.add(prefix, "time_dir_lin", (datasetSummary->get_time_dir_lin()).c_str(),true);
		kwl.add(prefix, "line_spacing", datasetSummary->get_line_spacing(),true);
		kwl.add(prefix, "pix_spacing", datasetSummary->get_pix_spacing(),true);
		kwl.add(prefix, "nlooks_az", datasetSummary->get_n_azilok(),true);
		kwl.add(prefix, "n_rnglok", datasetSummary->get_n_rnglok(),true);
		kwl.add(prefix, "zero_dop_range_time_f_pixel", datasetSummary->get_zero_dop_range_time_f_pixel(),true);
		kwl.add(prefix, "zero_dop_range_time_c_pixel", datasetSummary->get_zero_dop_range_time_c_pixel(),true);
		kwl.add(prefix, "zero_dop_range_time_l_pixel", datasetSummary->get_zero_dop_range_time_l_pixel(),true);
	}
	else
	{
		result = false;
	}
	
	ErsSarMapProjectionData *mapprojectiondata = get_ErsSarMapProjectionData();
	if ( (mapprojectiondata != NULL) && (result == true) )
	{
		kwl.add(prefix, "map_proj_des",(mapprojectiondata->get_map_proj_des()).c_str(),true);
		kwl.add(prefix, "num_lines",(double) mapprojectiondata->get_num_lines(),true);
		kwl.add(prefix, "num_pix",(double) mapprojectiondata->get_num_pix_in_line(),true);
		kwl.add(prefix, "first_line_first_pixel_lat",mapprojectiondata->get_first_line_first_pixel_lat(), true);
		kwl.add(prefix, "first_line_first_pixel_lon",mapprojectiondata->get_first_line_first_pixel_lon(), true);
		kwl.add(prefix, "first_line_last_pixel_lat",mapprojectiondata->get_first_line_last_pixel_lat(), true);
		kwl.add(prefix, "first_line_last_pixel_lon",mapprojectiondata->get_first_line_last_pixel_lon(), true);
		kwl.add(prefix, "last_line_first_pixel_lat",mapprojectiondata->get_last_line_first_pixel_lat(), true);
		kwl.add(prefix, "last_line_first_pixel_lon",mapprojectiondata->get_last_line_first_pixel_lon(), true);
		kwl.add(prefix, "last_line_last_pixel_lat",mapprojectiondata->get_last_line_last_pixel_lat(), true);
		kwl.add(prefix, "last_line_last_pixel_lon",mapprojectiondata->get_last_line_last_pixel_lon(), true);
	}
	else
	{
		result = false;
	}
	
	ErsSarPlatformPositionData *platformposition = get_ErsSarPlatformPositionData();
	if ( (mapprojectiondata != NULL) && (result == true) )
	{
		kwl.add(prefix, "neph", platformposition->get_ndata(),true);
		kwl.add(prefix, "eph_year", platformposition->get_year(),true);
		kwl.add(prefix, "eph_month", platformposition->get_month(),true);
		kwl.add(prefix, "eph_day", platformposition->get_day(),true);
		kwl.add(prefix, "eph_gmt_day", platformposition->get_gmt_day(),true);
		kwl.add(prefix, "eph_sec", platformposition->get_gmt_sec(),true);
		kwl.add(prefix, "eph_hr_angle", platformposition->get_hr_angle(),true);
		kwl.add(prefix, "eph_int", platformposition->get_data_int(),true);

		for (int i=0;i<platformposition->get_ndata();i++)
		{
			sprintf(name,"eph%i_posX",i);
			kwl.add(prefix, name,(platformposition->get_pos_vect()[i]).get_pos()[0],true);
			sprintf(name,"eph%i_posY",i);
			kwl.add(prefix, name,(platformposition->get_pos_vect()[i]).get_pos()[1],true);
			sprintf(name,"eph%i_posZ",i);
			kwl.add(prefix, name,(platformposition->get_pos_vect()[i]).get_pos()[2],true);

			sprintf(name,"eph%i_velX",i);
			kwl.add(prefix, name,(platformposition->get_pos_vect()[i]).get_vel()[0],true);
			sprintf(name,"eph%i_velY",i);
			kwl.add(prefix, name,(platformposition->get_pos_vect()[i]).get_vel()[1],true);
			sprintf(name,"eph%i_velZ",i);
			kwl.add(prefix, name,(platformposition->get_pos_vect()[i]).get_vel()[2],true);
		}
	}
	else
	{
		result = false;
	}
	/*
	 * Adding metadata necessary to the sensor model in the keywordlist
	 */
	ErsSarFacilityData *facilitydata = get_ErsSarFacilityData();
	if ( (facilitydata != NULL) && (result == true) )
	{
		kwl.add(prefix, "coef_ground_range_1",facilitydata->get_coef_ground_range_1(),true);
		kwl.add(prefix, "coef_ground_range_2",facilitydata->get_coef_ground_range_2(),true);
		kwl.add(prefix, "coef_ground_range_3",facilitydata->get_coef_ground_range_3(),true);
		kwl.add(prefix, "coef_ground_range_4",facilitydata->get_coef_ground_range_4(),true);
	}
	else
	{
		result = false;
	}
  
  /* 
  if (traceDebug())
  {
    ossimNotify(ossimNotifyLevel_DEBUG)
       << MODULE << " exit status = " << (result?"true":"false\n")
       << std::endl;
  }
  */

  return result;
}


ErsSarFacilityData * ErsSarLeader::get_ErsSarFacilityData()
{
	return (ErsSarFacilityData*)_records[ErsSarFacilityDataID];
}
ErsSarPlatformPositionData * ErsSarLeader::get_ErsSarPlatformPositionData()
{
	return (ErsSarPlatformPositionData*)_records[ErsSarPlatformPositionDataID];
}
ErsSarMapProjectionData * ErsSarLeader::get_ErsSarMapProjectionData()
{
	return (ErsSarMapProjectionData*)_records[ErsSarMapProjectionDataID];
}

ErsSarDataSetSummary * ErsSarLeader::get_ErsSarDataSetSummary()
{
	return (ErsSarDataSetSummary*)_records[ErsSarDataSetSummaryID];
}

ErsSarFileDescriptor * ErsSarLeader::get_ErsSarFileDescriptor()
{
	return (ErsSarFileDescriptor*)_records[ErsSarFileDescriptorID];
}
