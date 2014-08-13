for(;fromProjectionCount<=toProjectionCount; fromProjectionCount++){
  for(energyLoopCount = 0; energyLoopCount < cfg.energyLevels; energyLevel = cfg.minEnergy + ((cfg.maxEnergy-cfg.minEnergy)/(cfg.energyLevels-1))*(++energyLoopCount)){
    for(s = -SINOGRAMSIZE/2; s < SINOGRAMSIZE/2; s++){//s = detector element on line
      intensity[s+SINOGRAMSIZE/2] = precalculatedPhotonCounts[energyLoopCount];
      for(t = -COLS; t < COLS; t++){//t = ray length
        x = (int)( t*sinAngle + s*cosAngle + COLS/2);
        y = (int)(-t*cosAngle + s*sinAngle + COLS/2);
        if(x >= 0 && x < COLS && y >= 0 && y < COLS){//check if position is inside of image
          accumulatedAttenuationForPixel = 0.0;
          for(mat = MINMAT; mat <= MAXMAT; mat++){//mat is material ID
          	accumulatedAttenuationForPixel += getAttenuation(mat, energyLevel, x, y);
          }
          intensity[s+SINOGRAMSIZE/2] *= pow(EULER, -accumulatedAttenuationForPixel);
        }
      }
    result[projectionNumber][s+SINOGRAMSIZE/2] += intensity[s+SINOGRAMSIZE/2];
    }
  }
}