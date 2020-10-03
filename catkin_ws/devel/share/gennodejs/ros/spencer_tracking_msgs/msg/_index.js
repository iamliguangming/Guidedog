
"use strict";

let TrackedPerson = require('./TrackedPerson.js');
let DetectedPerson = require('./DetectedPerson.js');
let PersonTrajectoryEntry = require('./PersonTrajectoryEntry.js');
let ImmDebugInfos = require('./ImmDebugInfos.js');
let ImmDebugInfo = require('./ImmDebugInfo.js');
let TrackedGroup = require('./TrackedGroup.js');
let TrackedPerson2d = require('./TrackedPerson2d.js');
let PersonTrajectory = require('./PersonTrajectory.js');
let TrackedPersons = require('./TrackedPersons.js');
let CompositeDetectedPersons = require('./CompositeDetectedPersons.js');
let TrackedGroups = require('./TrackedGroups.js');
let TrackedPersons2d = require('./TrackedPersons2d.js');
let DetectedPersons = require('./DetectedPersons.js');
let TrackingTimingMetrics = require('./TrackingTimingMetrics.js');
let CompositeDetectedPerson = require('./CompositeDetectedPerson.js');

module.exports = {
  TrackedPerson: TrackedPerson,
  DetectedPerson: DetectedPerson,
  PersonTrajectoryEntry: PersonTrajectoryEntry,
  ImmDebugInfos: ImmDebugInfos,
  ImmDebugInfo: ImmDebugInfo,
  TrackedGroup: TrackedGroup,
  TrackedPerson2d: TrackedPerson2d,
  PersonTrajectory: PersonTrajectory,
  TrackedPersons: TrackedPersons,
  CompositeDetectedPersons: CompositeDetectedPersons,
  TrackedGroups: TrackedGroups,
  TrackedPersons2d: TrackedPersons2d,
  DetectedPersons: DetectedPersons,
  TrackingTimingMetrics: TrackingTimingMetrics,
  CompositeDetectedPerson: CompositeDetectedPerson,
};
